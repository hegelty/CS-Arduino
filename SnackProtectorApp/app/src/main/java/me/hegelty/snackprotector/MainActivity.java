package me.hegelty.snackprotector;

import android.app.Activity;
import android.os.AsyncTask;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.Switch;
import android.widget.TextView;

import com.google.firebase.messaging.FirebaseMessaging;

public class MainActivity extends Activity {
    private Button apply_button;
    private Button clear_button;
    private CheckBox led_checkbox;
    private CheckBox sound_checkbox;
    private Switch lock_switch;
    private TextView log_textview;
    private Button refresh_button;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        //토큰 정보
        FirebaseMessaging.getInstance().subscribeToTopic("snack");

        ShowStat showStat = new ShowStat();
        showStat.execute();
        ShowLog showLog = new ShowLog();
        showLog.execute();

        apply_button = findViewById(R.id.apply_button);
        clear_button = findViewById(R.id.reset_button);
        led_checkbox = findViewById(R.id.led_checkbox);
        sound_checkbox = findViewById(R.id.sound_checkbox);
        lock_switch = findViewById(R.id.lock_switch);
        log_textview = findViewById(R.id.log_view);
        refresh_button = findViewById(R.id.refresh_button);

        apply_button.setOnClickListener(new Button.OnClickListener() {
            @Override
            public void onClick(View view) {
                Integer stat=0;
                Boolean led = led_checkbox.isChecked();
                Boolean sound = sound_checkbox.isChecked();
                Boolean lock = lock_switch.isChecked();

                if(lock) {
                    if(led) {
                        if(sound) stat=4;
                        else stat=2;
                    }
                    else {
                        if(sound) stat=3;
                        else stat=1;
                    }
                }

                SetStat setStat = new SetStat(stat);
                setStat.execute();
            }
        });
        clear_button.setOnClickListener(new Button.OnClickListener() {
            @Override
            public void onClick(View view) {
                ClearLog clearLog = new ClearLog();
                clearLog.execute();
            }
        });
        refresh_button.setOnClickListener(new Button.OnClickListener() {
            @Override
            public void onClick(View view) {
                ShowLog showLog = new ShowLog();
                showLog.execute();
            }
        });
    }

    public class SetStat extends AsyncTask<Void, Void, Integer> {
        private Integer stat;
        public SetStat(Integer stat) {
            this.stat = stat;
        }

        @Override
        protected Integer doInBackground(Void... voids) {
            Integer result;
            RequestSender requestSender = new RequestSender();
            result = requestSender.setStat(stat);
            return result;
        }

        @Override
        protected void onPostExecute(Integer i) {
            super.onPostExecute(i);
            lock_switch.setChecked(false);
            led_checkbox.setChecked(false);
            sound_checkbox.setChecked(false);
            if(i != 0) lock_switch.setChecked(true);
            if(i==2 || i==4) led_checkbox.setChecked(true);
            if(i==3 || i==4) sound_checkbox.setChecked(true);
        }
    }

    public class ShowStat extends AsyncTask<Void, Void, Integer> {
        public ShowStat() {}
        @Override
        protected Integer doInBackground(Void... voids) {
            Integer result;
            RequestSender requestSender = new RequestSender();
            result = requestSender.getStat();
            return result;
        }

        @Override
        protected void onPostExecute(Integer i) {
            super.onPostExecute(i);
            lock_switch.setChecked(false);
            led_checkbox.setChecked(false);
            sound_checkbox.setChecked(false);
            if(i != 0) lock_switch.setChecked(true);
            if(i==2 || i==4) led_checkbox.setChecked(true);
            if(i==3 || i==4) sound_checkbox.setChecked(true);
        }
    }

    public class ShowLog extends AsyncTask<Void, Void, String> {
        public ShowLog() {}
        @Override
        protected String doInBackground(Void... voids) {
            String result;
            RequestSender requestSender = new RequestSender();
            result = requestSender.getLog();
            return result;
        }

        @Override
        protected void onPostExecute(String s) {
            super.onPostExecute(s);
            log_textview.setText(s);
        }
    }

    public class ClearLog extends AsyncTask<Void, Void, String> {
        public ClearLog() {}
        @Override
        protected String doInBackground(Void... voids) {
            Log.d("clearlog", "clear");
            String result;
            RequestSender requestSender = new RequestSender();
            result = requestSender.clearLog();
            return result;
        }

        @Override
        protected void onPostExecute(String s) {
            super.onPostExecute(s);
            log_textview.setText(s);
        }
    }
}