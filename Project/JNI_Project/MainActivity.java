package com.example.project;

import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    private TextView time;
    private TextView acc;
    private Button getResult;

    public native float[] getItem();
    static {
        System.loadLibrary("recognition");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        time = (TextView) findViewById(R.id.result_time);
        acc = (TextView) findViewById(R.id.result_acc);
        getResult = (Button) findViewById(R.id.getResult);

        getResult.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                float[] result = getItem();
                String tmp1 = String.format("%.0f", result[0]);
                String tmp2 = String.format("%.0f", result[1]);
                String num = String.format("%.3f", result[2]);
                time.setText("Elapsed time = " + tmp1 + "." +tmp2 + "sec");
                acc.setText("Accuracy = " + num);
            }
        });
    }
}