package com.example.a7invensun.gearvr_v1;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;

import java.io.IOException;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        GearVrTest test = new GearVrTest();
        try {
            test.Init(this);
            test.StartCamera(this);
        } catch (IOException e) {
            e.printStackTrace();
        }
//        setTitle("SDK test"+ test.getErrorMsg());
        Log.i("try to test SDK","sdk test");
    }
}
