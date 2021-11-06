package me.hegelty.snackprotector;

import android.util.Log;

import org.jsoup.Connection;
import org.jsoup.Jsoup;

public class RequestSender {
    public int setStat(int statnum) {
        try {
            String response = Jsoup.connect("http://52.253.107.159:8000/set/"+statnum)
                    .ignoreContentType(true).get().text();
            return Integer.parseInt(response);
        } catch (Exception e) {
            e.printStackTrace();
            return -1;
        }
    }

    public String clearLog() {
        try {
            String response = Jsoup.connect("http://52.253.107.159:8000/clear")
                    .method(Connection.Method.POST)
                    .ignoreContentType(true).post().text();
            return response;
        } catch (Exception e) {
            e.printStackTrace();
            return "error";
        }
    }

    public String getLog() {
        try {
            String response = Jsoup.connect("http://52.253.107.159:8000/log")
                    .ignoreContentType(true)
                    .get().text().replaceAll("\"","").replaceAll("\\\\n","\n");
            return response;
        } catch (Exception e) {
            e.printStackTrace();
            return "error";
        }
    }

    public int getStat() {
        try {
            Log.d("getStat","getStat");
            String response = Jsoup.connect("http://52.253.107.159:8000/show")
                    .ignoreContentType(true).get().text();
            Log.d("getStat",response);
            return Integer.parseInt(response);
        } catch (Exception e) {
            e.printStackTrace();
            return -1;
        }
    }
}
