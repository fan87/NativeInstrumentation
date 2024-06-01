package me.fan87.nativeinstrumentation;

import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.Test;

public class GetSetFieldTest {

    private static String field = "hello";
    private static int intField = 0;
    private static byte byteField = 0;
    private static short shortField = 0;
    private static long longField = 0;
    private static boolean booleanField = false;
    private static double doubleField = 0;
    private static float floatField = 0;
    
    private String      _field = "hello";
    private int         _intField = 0;
    private byte        _byteField = 0;
    private short       _shortField = 0;
    private long        _longField = 0;
    private boolean     _booleanField = false;
    private double      _doubleField = 0;
    private float       _floatField = 0;
    
    
    
    @Test
    public void setStaticField() {
        NativeInstrumentation.setStaticField(GetSetFieldTest.class, "field", "world");
        Assertions.assertEquals("world", field);
        NativeInstrumentation.setStaticField(GetSetFieldTest.class, "intField", 1);
        Assertions.assertEquals(1, intField);
        NativeInstrumentation.setStaticField(GetSetFieldTest.class, "byteField", 1);
        Assertions.assertEquals(1, byteField);
        NativeInstrumentation.setStaticField(GetSetFieldTest.class, "shortField", 1);
        Assertions.assertEquals(1, shortField);
        NativeInstrumentation.setStaticField(GetSetFieldTest.class, "longField", 1);
        Assertions.assertEquals(1, longField);
        NativeInstrumentation.setStaticField(GetSetFieldTest.class, "booleanField", true);
        Assertions.assertTrue(booleanField);
        NativeInstrumentation.setStaticField(GetSetFieldTest.class, "doubleField", 1);
        Assertions.assertEquals(1, doubleField);
        NativeInstrumentation.setStaticField(GetSetFieldTest.class, "floatField", 1);
        Assertions.assertEquals(1, floatField);
    }

    // Undefined behaviors test
    @Test
    public void setStaticFieldOverflow() {
        booleanField = false;
        NativeInstrumentation.setStaticField(GetSetFieldTest.class, "booleanField", 1);
        Assertions.assertTrue(booleanField);
        NativeInstrumentation.setStaticField(GetSetFieldTest.class, "booleanField", 0);
        Assertions.assertFalse(booleanField);
        NativeInstrumentation.setStaticField(GetSetFieldTest.class, "booleanField", 200000);
        Assertions.assertFalse(booleanField);

        // Ok.. what the heck?
        // This works, I guess...
        // (There are actually a lot of undefined behavior with setStaticField, worth exploring)
        NativeInstrumentation.setStaticField(GetSetFieldTest.class, "field", new byte[10000]);
        Assertions.assertArrayEquals(new byte[10000], (byte[]) ((Object) field));
    }

    @Test
    public void getStaticField() {
        field = "Hi!";
        Assertions.assertEquals("Hi!", NativeInstrumentation.getStaticField(GetSetFieldTest.class, "field"));

        intField = 2;
        Assertions.assertEquals(2, NativeInstrumentation.getStaticField(GetSetFieldTest.class, "intField"));

        byteField = 2;
        Assertions.assertEquals(byteField, NativeInstrumentation.getStaticField(GetSetFieldTest.class, "byteField"));

        shortField = 2;
        Assertions.assertEquals(shortField, NativeInstrumentation.getStaticField(GetSetFieldTest.class, "shortField"));

        longField = 2;
        Assertions.assertEquals(longField, NativeInstrumentation.getStaticField(GetSetFieldTest.class, "longField"));

        doubleField = 2;
        Assertions.assertEquals(doubleField, NativeInstrumentation.getStaticField(GetSetFieldTest.class, "doubleField"));

        floatField = 2;
        Assertions.assertEquals(floatField, NativeInstrumentation.getStaticField(GetSetFieldTest.class, "floatField"));

        booleanField = true;
        Assertions.assertEquals(true, NativeInstrumentation.getStaticField(GetSetFieldTest.class, "booleanField"));
    }

    
    @Test
    public void setField() {
        NativeInstrumentation.setField(GetSetFieldTest.class, this, "_field", "world");
        Assertions.assertEquals("world", _field);
        NativeInstrumentation.setField(GetSetFieldTest.class, this, "_intField", 1);
        Assertions.assertEquals(1, _intField);
        NativeInstrumentation.setField(GetSetFieldTest.class, this, "_byteField", 1);
        Assertions.assertEquals(1, _byteField);
        NativeInstrumentation.setField(GetSetFieldTest.class, this, "_shortField", 1);
        Assertions.assertEquals(1, _shortField);
        NativeInstrumentation.setField(GetSetFieldTest.class, this, "_longField", 1);
        Assertions.assertEquals(1, _longField);
        NativeInstrumentation.setField(GetSetFieldTest.class, this, "_booleanField", true);
        Assertions.assertTrue(_booleanField);
        NativeInstrumentation.setField(GetSetFieldTest.class, this, "_doubleField", 1);
        Assertions.assertEquals(1, _doubleField);
        NativeInstrumentation.setField(GetSetFieldTest.class, this, "_floatField", 1);
        Assertions.assertEquals(1, _floatField);
    }

    // Undefined behaviors test
    @Test
    public void setFieldOverflow() {
        _booleanField = false;
        NativeInstrumentation.setField(GetSetFieldTest.class, this, "_booleanField", 1);
        Assertions.assertTrue(_booleanField);
        NativeInstrumentation.setField(GetSetFieldTest.class, this, "_booleanField", 0);
        Assertions.assertFalse(_booleanField);
        NativeInstrumentation.setField(GetSetFieldTest.class, this, "_booleanField", 200000);
        Assertions.assertFalse(_booleanField);
        NativeInstrumentation.setField(GetSetFieldTest.class, this, "_field", new byte[10000]);
        Assertions.assertArrayEquals(new byte[10000], (byte[]) ((Object) field));
    }

    @Test
    public void getField() {
        _field = "Hi!";
        Assertions.assertEquals("Hi!", NativeInstrumentation.getField(GetSetFieldTest.class, this, "_field"));

        _intField = 2;
        Assertions.assertEquals(2, NativeInstrumentation.getField(GetSetFieldTest.class, this, "_intField"));

        _byteField = 2;
        Assertions.assertEquals(_byteField, NativeInstrumentation.getField(GetSetFieldTest.class, this, "_byteField"));

        _shortField = 2;
        Assertions.assertEquals(_shortField, NativeInstrumentation.getField(GetSetFieldTest.class, this, "_shortField"));

        _longField = 2;
        Assertions.assertEquals(_longField, NativeInstrumentation.getField(GetSetFieldTest.class, this, "_longField"));

        _doubleField = 2;
        Assertions.assertEquals(_doubleField, NativeInstrumentation.getField(GetSetFieldTest.class, this, "_doubleField"));

        _floatField = 2;
        Assertions.assertEquals(_floatField, NativeInstrumentation.getField(GetSetFieldTest.class, this, "_floatField"));

        _booleanField = true;
        Assertions.assertEquals(true, NativeInstrumentation.getField(GetSetFieldTest.class, this, "_booleanField"));
    }


}
