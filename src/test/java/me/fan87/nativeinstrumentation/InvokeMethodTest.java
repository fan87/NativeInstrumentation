package me.fan87.nativeinstrumentation;

import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.Test;

public class InvokeMethodTest {

    public static class InvokeMethodTestNonStatic {
        public static InvokeMethodTestNonStatic instance = new InvokeMethodTestNonStatic();
        private String getValue(String value) { return value; }
        private int getIntValue(int value) { return value; }
        private byte getByteValue(byte value) { return value; }
        private char getCharValue(char value) { return value; }
        private short getShortValue(short value) { return value; }
        private long getLongValue(long value) { return value; }
        private boolean getBooleanValue(boolean value) { return value; }
        private float getFloatValue(float value) { return value; }
        private double getDoubleValue(double value) { return value; }

        private String getComplicatedValueA(String stringValue, String[] array, double doubleValue) {
            return stringValue + "_" + array.length + "_" + doubleValue;
        }
        private String[] getComplicatedValueB(String stringValue, String[] array, double doubleValue) {
            String[] output = new String[array.length];
            for (int i = 0; i < output.length; i++) {
                output[i] = stringValue + "_" + array.length + "_" + doubleValue + "_" + i;
            }
            return output;
        }
        private void modifyIncomingValue(String stringValue, String[] array, double doubleValue) {
            for (int i = 0; i < array.length; i++) {
                array[i] = stringValue + "_" + array.length + "_" + doubleValue + "_" + i;
            }
        }
    }

    private static String getValue(String value) { return value; }
    private static int getIntValue(int value) { return value; }
    private static byte getByteValue(byte value) { return value; }
    private static char getCharValue(char value) { return value; }
    private static short getShortValue(short value) { return value; }
    private static long getLongValue(long value) { return value; }
    private static boolean getBooleanValue(boolean value) { return value; }
    private static float getFloatValue(float value) { return value; }
    private static double getDoubleValue(double value) { return value; }

    private static String getComplicatedValueA(String stringValue, String[] array, double doubleValue) {
        return stringValue + "_" + array.length + "_" + doubleValue;
    }
    private static String[] getComplicatedValueB(String stringValue, String[] array, double doubleValue) {
        String[] output = new String[array.length];
        for (int i = 0; i < output.length; i++) {
            output[i] = stringValue + "_" + array.length + "_" + doubleValue + "_" + i;
        }
        return output;
    }
    private static void modifyIncomingValue(String stringValue, String[] array, double doubleValue) {
        for (int i = 0; i < array.length; i++) {
            array[i] = stringValue + "_" + array.length + "_" + doubleValue + "_" + i;
        }
    }


    @Test
    public void invokeStaticBasicMethod() {
        Assertions.assertEquals(5, NativeInstrumentation.invokeStaticMethodS(InvokeMethodTest.class, "getIntValue", "(I)I", 5));
        Assertions.assertEquals((byte) 5, NativeInstrumentation.invokeStaticMethodS(InvokeMethodTest.class, "getByteValue", "(B)B", (byte) 5));
        Assertions.assertEquals((short) 5, NativeInstrumentation.invokeStaticMethodS(InvokeMethodTest.class, "getShortValue", "(S)S", (short) 5));
        Assertions.assertEquals(5L, NativeInstrumentation.invokeStaticMethodS(InvokeMethodTest.class, "getLongValue", "(J)J", 5L));
        Assertions.assertEquals(5f, NativeInstrumentation.invokeStaticMethodS(InvokeMethodTest.class, "getFloatValue", "(F)F", 5f));
        Assertions.assertEquals(5.0, NativeInstrumentation.invokeStaticMethodS(InvokeMethodTest.class, "getDoubleValue", "(D)D", 5.0));
        Assertions.assertEquals('5', NativeInstrumentation.invokeStaticMethodS(InvokeMethodTest.class, "getCharValue", "(C)C", '5'));
        Assertions.assertEquals(false, NativeInstrumentation.invokeStaticMethodS(InvokeMethodTest.class, "getBooleanValue", "(Z)Z", false));
        Assertions.assertEquals("hello", NativeInstrumentation.invokeStaticMethodS(InvokeMethodTest.class, "getValue", "(Ljava/lang/String;)Ljava/lang/String;", "hello"));
    }
    @Test
    public void invokeStaticComplicatedMethod() {
        Assertions.assertEquals("hello_5_5.1", NativeInstrumentation.invokeStaticMethodS(InvokeMethodTest.class, "getComplicatedValueA", "(Ljava/lang/String;[Ljava/lang/String;D)Ljava/lang/String;", "hello", new String[5], 5.1));
        Assertions.assertArrayEquals(getComplicatedValueB("hello", new String[5], 5.1), (Object[]) NativeInstrumentation.invokeStaticMethodS(InvokeMethodTest.class, "getComplicatedValueB", "(Ljava/lang/String;[Ljava/lang/String;D)[Ljava/lang/String;", "hello", new String[5], 5.1));
        String[] array = new String[5];
        NativeInstrumentation.invokeStaticMethodS(InvokeMethodTest.class, "modifyIncomingValue", "(Ljava/lang/String;[Ljava/lang/String;D)V", "hello", array, 5.1);
        Assertions.assertArrayEquals(getComplicatedValueB("hello", new String[5], 5.1), array);
    }
    @Test
    public void invokeBasicMethod() {
        Assertions.assertEquals(5, NativeInstrumentation.invokeMethodS(InvokeMethodTestNonStatic.class, InvokeMethodTestNonStatic.instance, "getIntValue", "(I)I", 5));
        Assertions.assertEquals((byte) 5, NativeInstrumentation.invokeMethodS(InvokeMethodTestNonStatic.class, InvokeMethodTestNonStatic.instance, "getByteValue", "(B)B", (byte) 5));
        Assertions.assertEquals((short) 5, NativeInstrumentation.invokeMethodS(InvokeMethodTestNonStatic.class, InvokeMethodTestNonStatic.instance, "getShortValue", "(S)S", (short) 5));
        Assertions.assertEquals(5L, NativeInstrumentation.invokeMethodS(InvokeMethodTestNonStatic.class, InvokeMethodTestNonStatic.instance, "getLongValue", "(J)J", 5L));
        Assertions.assertEquals(5f, NativeInstrumentation.invokeMethodS(InvokeMethodTestNonStatic.class, InvokeMethodTestNonStatic.instance, "getFloatValue", "(F)F", 5f));
        Assertions.assertEquals(5.0, NativeInstrumentation.invokeMethodS(InvokeMethodTestNonStatic.class, InvokeMethodTestNonStatic.instance, "getDoubleValue", "(D)D", 5.0));
        Assertions.assertEquals('5', NativeInstrumentation.invokeMethodS(InvokeMethodTestNonStatic.class, InvokeMethodTestNonStatic.instance, "getCharValue", "(C)C", '5'));
        Assertions.assertEquals(false, NativeInstrumentation.invokeMethodS(InvokeMethodTestNonStatic.class, InvokeMethodTestNonStatic.instance, "getBooleanValue", "(Z)Z", false));
        Assertions.assertEquals("hello", NativeInstrumentation.invokeMethodS(InvokeMethodTestNonStatic.class, InvokeMethodTestNonStatic.instance, "getValue", "(Ljava/lang/String;)Ljava/lang/String;", "hello"));
    }
    @Test
    public void invokeComplicatedMethod() {
        Assertions.assertEquals("hello_5_5.1", NativeInstrumentation.invokeMethodS(InvokeMethodTestNonStatic.class, InvokeMethodTestNonStatic.instance, "getComplicatedValueA", "(Ljava/lang/String;[Ljava/lang/String;D)Ljava/lang/String;", "hello", new String[5], 5.1));
        Assertions.assertArrayEquals(getComplicatedValueB("hello", new String[5], 5.1), (Object[]) NativeInstrumentation.invokeMethodS(InvokeMethodTestNonStatic.class, InvokeMethodTestNonStatic.instance, "getComplicatedValueB", "(Ljava/lang/String;[Ljava/lang/String;D)[Ljava/lang/String;", "hello", new String[5], 5.1));
        String[] array = new String[5];
        NativeInstrumentation.invokeMethodS(InvokeMethodTestNonStatic.class, InvokeMethodTestNonStatic.instance, "modifyIncomingValue", "(Ljava/lang/String;[Ljava/lang/String;D)V", "hello", array, 5.1);
        Assertions.assertArrayEquals(getComplicatedValueB("hello", new String[5], 5.1), array);
    }
    @Test
    public void exceptionCheck() {
        Assertions.assertEquals(
            "failed to find method test1 with signature ()V",
            Assertions.assertThrows(NoSuchMethodException.class, () -> {
                NativeInstrumentation.invokeStaticMethodS(InvokeMethodTest.class, "test1", "()V");
            }).getMessage()
        );
        Assertions.assertEquals(
            "failed to find method test2 with signature (Ljava/lang/String;)Z",
            Assertions.assertThrows(NoSuchMethodException.class, () -> {
                NativeInstrumentation.invokeMethodS(InvokeMethodTest.class, this, "test2", "(Ljava/lang/String;)Z");
            }).getMessage()
        );
    }

}
