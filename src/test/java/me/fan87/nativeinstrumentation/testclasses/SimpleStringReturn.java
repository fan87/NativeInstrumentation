package me.fan87.nativeinstrumentation.testclasses;

@SuppressWarnings("ALL")
public class SimpleStringReturn {

    public static String getText() {
        return "text_before_modification";
    }

}
