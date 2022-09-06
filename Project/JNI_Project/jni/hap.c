#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <arm_neon.h>
#include <pthread.h>
#include <hap.h>

#define IMG_SIZE 784
#define IMG_COUNT 50000
#define DIGIT_COUNT 10
#define sigmoid(x) (1 / (1 + exp(-x)))

typedef struct {
  float *images;
  float *network;
  float *confidences;
  int *labels;
  int start;
  int finish;
}thread_args;

int timespec_subtract(struct timespec*, struct timespec*, struct timespec*);
void recogn(float * images, float * network, int depth, int size, int * labels, float * confidences);
void *re(void *arg);

JNIEXPORT jfloatArray JNICALL Java_com_example_project_MainActivity_getItem
    (JNIEnv *env, jobject this){
    jfloatArray retArr = (*env)->NewFloatArray(env, 3);
    float arr[3] = {};

    float *images, *network, *confidences, accuracy;
    int *labels;
    int *labels_ans;
    int i, correct, total_network_size, depth, size;
    FILE *io_file;
    struct timespec start, end, spent;

    images = (float *)malloc(sizeof(float)*IMG_COUNT*IMG_SIZE);
    labels = (int *)malloc(sizeof(int)*IMG_COUNT);
    labels_ans = (int *)malloc(sizeof(int)*IMG_COUNT);
    confidences = (float *)malloc(sizeof(float)*IMG_COUNT);

    io_file = fopen("/data/local/medium.network", "r");
    fread(&depth, sizeof(int), 1, io_file);
    fread(&size, sizeof(int), 1, io_file);
    total_network_size = (IMG_SIZE * size + size) + (depth - 1) * (size * size + size) + size  * DIGIT_COUNT + DIGIT_COUNT;
    network = (float *)malloc(sizeof(float) * (total_network_size));
    fread(network, sizeof(float), total_network_size, io_file);
    fclose(io_file);

    io_file = fopen("data/local/MNIST_image.bin", "r");
    fread(images, sizeof(float), IMG_COUNT * IMG_SIZE, io_file);
    fclose(io_file);

    io_file = fopen("data/local/MNIST_label.bin", "r");
    fread(labels_ans, sizeof(int), IMG_COUNT, io_file);
    fclose(io_file);

    clock_gettime(CLOCK_MONOTONIC, &start);
    recogn(images, network, depth, size, labels, confidences);
    clock_gettime(CLOCK_MONOTONIC, &end);
    timespec_subtract(&spent, &end, &start);

    correct = 0;
    for(i = 0; i <IMG_COUNT; i++){
      if(labels_ans[i] == labels[i]) correct++;
    }
    accuracy = (float)correct / (float)IMG_COUNT;
    arr[0] = spent.tv_sec;
    arr[1] = spent.tv_nsec/1000/1000;
    arr[2] = accuracy;
    (*env)->SetFloatArrayRegion(env, retArr, 0, 3, arr);
    return retArr;
}

int timespec_subtract(struct timespec* result, struct timespec *x, struct timespec *y) {
  /* Perform the carry for the later subtraction by updating y. */
  if (x->tv_nsec < y->tv_nsec) {
    int nsec = (y->tv_nsec - x->tv_nsec) / 1000000000 + 1;
    y->tv_nsec -= 1000000000 * nsec;
    y->tv_sec += nsec;
  }
  if (x->tv_nsec - y->tv_nsec > 1000000000) {
    int nsec = (x->tv_nsec - y->tv_nsec) / 1000000000;
    y->tv_nsec += 1000000000 * nsec;
    y->tv_sec -= nsec;
  }

  /* Compute the time remaining to wait.
     tv_nsec is certainly positive. */
  result->tv_sec = x->tv_sec - y->tv_sec;
  result->tv_nsec = x->tv_nsec - y->tv_nsec;

  /* Return 1 if result is negative. */
  return x->tv_sec < y->tv_sec;
}

void recogn(float * images, float * network, int depth, int size, int * labels, float * confidences)
{
  int thread_num = 2;
  int s = IMG_COUNT / thread_num;

  pthread_t t[thread_num];
  thread_args thr[thread_num];

  for(int i = 0; i < thread_num; i++){
    thr[i].images = images;
    thr[i].network = network;
    thr[i].confidences = confidences;
    thr[i].labels = labels;
    thr[i].start = s * i;
    thr[i].finish = thr[i].start + s;
  }

  for(int i = 0; i < thread_num; i++){
    pthread_create(&t[i], NULL, re, &thr[i]);
  }

  for(int i = 0; i < thread_num; i++){
    pthread_join(t[i], NULL);
  }
}

void *re(void *arg)
{
  thread_args *a = (thread_args *)arg;

  unsigned int i, j, x, y, tmp, tmp_i, tmp_j, idx, label;
  int start, finish;
  int depth = 3;
  int size = 512;
  float max;

  float *hidden_layers, **weights, **biases;
  float *images = a->images;
  float *confidences = a->confidences;
  int *labels = a->labels;
  start = a->start;
  finish = a->finish;

  //IMG_SIZE * size
  unsigned int temp = 401408;
  //size * 2(1 << 10)
  unsigned int tmp_h = 1024;
  //size * size + size((1 << 18) + 512)
  unsigned int tmp_s = 262656;

  float32x4_t input_vec1, input_vec2;
  float32x4_t weights_vec1, weights_vec2;
  float32x4_t result_vec1, result_vec2;

  hidden_layers = (float *)malloc(sizeof(float) * depth << 9);
  weights = (float **)malloc(sizeof(float *) * 4);
  biases = (float **)malloc(sizeof(float *) * 4);

  // Set pointers for weights and biases
  // 1. Input layer
  weights[0] = a->network;
  biases[0] = weights[0] + temp;
  // 2. Hidden layers
  for(i = 1; i < depth; i++)
  {
    weights[i] = a->network + size + temp + tmp_s * (i - 1);
    biases[i] = weights[i] + (1 << 18);
  }
  // 3. Output layer
  weights[3] = weights[2] + tmp_s;
  biases[3] = weights[3] + (5 << 10);

  // Recognize numbers
  tmp_i = start * IMG_SIZE;
  for(i = start; i < finish; i++)
  {
    float *input = images + tmp_i;
    float output[DIGIT_COUNT];
    max = 0;
    label = 0;

    // From the input layer to the first hidden layer
    tmp = 0;
    for(x = 0; x < size; x++)
    {
      float sum = biases[0][x];
      result_vec1 = vdupq_n_f32(0);
      result_vec2 = vdupq_n_f32(0);
      idx = 0;

      for(y = 0; y < 98; y++)
      {
        input_vec1 = vld1q_f32(input + idx);
        input_vec2 = vld1q_f32(input + idx + 4);
        weights_vec1 = vld1q_f32(weights[0] + tmp + idx);
        weights_vec2 = vld1q_f32(weights[0] + tmp + idx + 4);
        result_vec1 = vmlaq_f32(result_vec1, input_vec1, weights_vec1);
        result_vec2 = vmlaq_f32(result_vec2, input_vec2, weights_vec2);

        idx += 8;
      }
      sum += vgetq_lane_f32(result_vec1, 0) + vgetq_lane_f32(result_vec1, 1) + vgetq_lane_f32(result_vec1, 2) + vgetq_lane_f32(result_vec1, 3) + vgetq_lane_f32(result_vec2, 0) + vgetq_lane_f32(result_vec2, 1) + vgetq_lane_f32(result_vec2, 2) + vgetq_lane_f32(result_vec2, 3);
      hidden_layers[x] = sigmoid(sum);
      tmp += IMG_SIZE;
    }

    // Between hidden layers
    tmp_j = 0;
    for(j = 1; j < depth; j++)
    {
      tmp = 0;
      for(x = 0; x < size; x++)
      {
        float sum = biases[j][x];
        result_vec1 = vdupq_n_f32(0);
        result_vec2 = vdupq_n_f32(0);
        idx = 0;

        for(y = 0; y < 64; y++)
        {
          input_vec1 = vld1q_f32(hidden_layers + tmp_j + idx);
          input_vec2 = vld1q_f32(hidden_layers + tmp_j + idx + 4);
          weights_vec1 = vld1q_f32(weights[j] + tmp + idx);
          weights_vec2 = vld1q_f32(weights[j] + tmp + idx + 4);
          result_vec1 = vmlaq_f32(result_vec1, input_vec1, weights_vec1);
          result_vec2 = vmlaq_f32(result_vec2, input_vec2, weights_vec2);

          idx += 8;
        }
        sum += vgetq_lane_f32(result_vec1, 0) + vgetq_lane_f32(result_vec1, 1) + vgetq_lane_f32(result_vec1, 2) + vgetq_lane_f32(result_vec1, 3) + vgetq_lane_f32(result_vec2, 0) + vgetq_lane_f32(result_vec2, 1) + vgetq_lane_f32(result_vec2, 2) + vgetq_lane_f32(result_vec2, 3);
        hidden_layers[size + tmp_j + x] = sigmoid(sum);
        tmp += size;
      }
      tmp_j += size;
    }

    // From the last hidden layer to the output layer
    tmp = 0;
    for(x = 0; x < DIGIT_COUNT; x++)
    {
      float sum = biases[3][x];
      result_vec1 = vdupq_n_f32(0);
      result_vec2 = vdupq_n_f32(0);
      idx = 0;

      for(y = 0; y < 64; y++)
      {
        input_vec1 = vld1q_f32(hidden_layers + tmp_h + idx);
        input_vec2 = vld1q_f32(hidden_layers + tmp_h + idx + 4);
        weights_vec1 = vld1q_f32(weights[3] + tmp + idx);
        weights_vec2 = vld1q_f32(weights[3] + tmp + idx + 4);
        result_vec1 = vmlaq_f32(result_vec1, input_vec1, weights_vec1);
        result_vec2 = vmlaq_f32(result_vec2, input_vec2, weights_vec2);

        idx += 8;
      }
      sum += vgetq_lane_f32(result_vec1, 0) + vgetq_lane_f32(result_vec1, 1) + vgetq_lane_f32(result_vec1, 2) + vgetq_lane_f32(result_vec1, 3) + vgetq_lane_f32(result_vec2, 0) + vgetq_lane_f32(result_vec2, 1) + vgetq_lane_f32(result_vec2, 2) + vgetq_lane_f32(result_vec2, 3);
      output[x] = sigmoid(sum);
      tmp += size;

      if((int)output[x]){
        max = output[x];
        label = x;
      }
      if(output[x] - max > 0)
      {
        label = x;
        max = output[x];
      }
    }

    // Store the result
    confidences[i] = max;
    labels[i] = label;
    tmp_i += IMG_SIZE;
  }
  return NULL;
}
