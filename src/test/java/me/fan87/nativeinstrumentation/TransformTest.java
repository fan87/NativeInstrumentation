package me.fan87.nativeinstrumentation;

import me.fan87.nativeinstrumentation.testclasses.*;
import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.Test;
import org.junit.platform.commons.logging.Logger;
import org.junit.platform.commons.logging.LoggerFactory;
import org.objectweb.asm.ClassReader;
import org.objectweb.asm.ClassWriter;
import org.objectweb.asm.tree.AbstractInsnNode;
import org.objectweb.asm.tree.ClassNode;
import org.objectweb.asm.tree.LdcInsnNode;
import org.objectweb.asm.tree.MethodNode;

import java.lang.instrument.ClassDefinition;
import java.lang.instrument.ClassFileTransformer;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicReference;

public class TransformTest {
    NativeInstrumentation instrumentation = new NativeInstrumentation();

    @Test
    public void transformChangeValueStringReturnRetransform() {
        Assertions.assertEquals("text_before_modification", SimpleStringReturn.getText());
        ClassFileTransformer transformer = (loader, className, classBeingRedefined, protectionDomain, classfileBuffer) -> {
            if (className.equals("me/fan87/nativeinstrumentation/testclasses/SimpleStringReturn")) {
                ClassNode node = new ClassNode();
                ClassReader reader = new ClassReader(classfileBuffer);
                reader.accept(node, ClassReader.EXPAND_FRAMES);
                for (MethodNode method : node.methods) {
                    if (method.name.equals("getText")) {
                        for (AbstractInsnNode instruction : method.instructions) {
                            if (instruction instanceof LdcInsnNode) {
                                ((LdcInsnNode) instruction).cst = "text_after_modification";
                            }
                        }
                    }
                }
                ClassWriter writer = new ClassWriter(ClassWriter.COMPUTE_FRAMES);
                node.accept(writer);
                return writer.toByteArray();
            }
            return null;
        };
        instrumentation.addTransformer(transformer, true);
        instrumentation.retransformClasses(SimpleStringReturn.class);
        instrumentation.removeTransformer(transformer);
        Assertions.assertEquals("text_after_modification", SimpleStringReturn.getText());
    }
    @Test
    public void transformChangeValueStringReturnRedefine() throws ClassNotFoundException {
        Assertions.assertEquals("text_before_modification", SimpleStringReturn2.getText());
        AtomicReference<byte[]> content = new AtomicReference<>(new byte[0]);
        ClassFileTransformer transformer = (loader, className, classBeingRedefined, protectionDomain, classfileBuffer) -> {
            if (className.equals("me/fan87/nativeinstrumentation/testclasses/SimpleStringReturn2")) {
                content.set(classfileBuffer);
            }
            return null;
        };
        instrumentation.addTransformer(transformer, true);
        instrumentation.retransformClasses(SimpleStringReturn2.class);
        instrumentation.removeTransformer(transformer);
        Assertions.assertNotEquals(0, content.get().length);
        ClassNode node = new ClassNode();
        ClassReader reader = new ClassReader(content.get());
        reader.accept(node, ClassReader.EXPAND_FRAMES);
        for (MethodNode method : node.methods) {
            if (method.name.equals("getText")) {
                for (AbstractInsnNode instruction : method.instructions) {
                    if (instruction instanceof LdcInsnNode) {
                        ((LdcInsnNode) instruction).cst = "text_after_modification";
                    }
                }
            }
        }
        ClassWriter writer = new ClassWriter(ClassWriter.COMPUTE_FRAMES);
        node.accept(writer);
        instrumentation.redefineClasses(new ClassDefinition(SimpleStringReturn2.class, writer.toByteArray()));
        Assertions.assertEquals("text_after_modification", SimpleStringReturn2.getText());
    }
    @Test
    public void transformChangeValueStringReturn() {
        Logger logger = LoggerFactory.getLogger(TransformTest.class);
        for (Class<?> allLoadedClass : instrumentation.getAllLoadedClasses()) {
            if (allLoadedClass.getName().startsWith("me.fan87.nativeinstrumentation.testclasses.SimpleStringReturn_DoNotLoad")) {
                logger.warn(() -> allLoadedClass.getName() + " is already loaded, the test will be bypassed, this should only happen with Gradle");
                return;
            }
        }
        ClassFileTransformer transformer = (loader, className, classBeingRedefined, protectionDomain, classfileBuffer) -> {
            if (className.equals("me/fan87/nativeinstrumentation/testclasses/SimpleStringReturn_DoNotLoad")) {
                ClassNode node = new ClassNode();
                ClassReader reader = new ClassReader(classfileBuffer);
                reader.accept(node, ClassReader.EXPAND_FRAMES);
                for (MethodNode method : node.methods) {
                    if (method.name.equals("getText")) {
                        for (AbstractInsnNode instruction : method.instructions) {
                            if (instruction instanceof LdcInsnNode) {
                                ((LdcInsnNode) instruction).cst = "text_after_modification";
                            }
                        }
                    }
                }
                ClassWriter writer = new ClassWriter(ClassWriter.COMPUTE_FRAMES);
                node.accept(writer);
                return writer.toByteArray();
            }
            return null;
        };
        instrumentation.addTransformer(transformer);
        Assertions.assertEquals("text_after_modification", SimpleStringReturn_DoNotLoad.getText());
    }
    @Test
    public void retransformFlag() throws InstantiationException, IllegalAccessException {
        AtomicBoolean transformed1 = new AtomicBoolean(false);
        AtomicBoolean transformed2 = new AtomicBoolean(false);
        AtomicBoolean transformed3 = new AtomicBoolean(false);
        ClassFileTransformer transformer1 = (loader, className, classBeingRedefined, protectionDomain, classfileBuffer) -> {
            if (className.equals("me/fan87/nativeinstrumentation/testclasses/EmptyClass_DoNotLoad_1")) {
                transformed1.set(true);
            }
            return null;
        };
        ClassFileTransformer transformer2 = (loader, className, classBeingRedefined, protectionDomain, classfileBuffer) -> {
            if (className.equals("me/fan87/nativeinstrumentation/testclasses/EmptyClass_DoNotLoad_2")) {
                transformed2.set(true);
            }
            return null;
        };
        ClassFileTransformer transformer3 = (loader, className, classBeingRedefined, protectionDomain, classfileBuffer) -> {
            if (className.equals("me/fan87/nativeinstrumentation/testclasses/EmptyClass_DoNotLoad_3")) {
                transformed3.set(true);
            }
            return null;
        };
        instrumentation.addTransformer(transformer1);
        instrumentation.addTransformer(transformer2, true);
        instrumentation.addTransformer(transformer3, false);

        Logger logger = LoggerFactory.getLogger(TransformTest.class);
        for (Class<?> allLoadedClass : instrumentation.getAllLoadedClasses()) {
            if (allLoadedClass.getName().startsWith("me.fan87.nativeinstrumentation.testclasses.EmptyClass_DoNotLoad_")) {
                logger.warn(() -> allLoadedClass.getName() + " is already loaded, the first part of the test will now be bypassed, this should only happen with Gradle");
                transformed1.set(true);
                transformed2.set(true);
                transformed3.set(true);
                break;
            }
        }
        EmptyClass_DoNotLoad_1.class.newInstance();
        EmptyClass_DoNotLoad_2.class.newInstance();
        EmptyClass_DoNotLoad_3.class.newInstance();

        Assertions.assertTrue(transformed1.get());
        Assertions.assertTrue(transformed2.get());
        Assertions.assertTrue(transformed3.get());
        transformed1.set(false);
        transformed2.set(false);
        transformed3.set(false);
        instrumentation.retransformClasses(EmptyClass_DoNotLoad_1.class, EmptyClass_DoNotLoad_2.class, EmptyClass_DoNotLoad_3.class);
        Assertions.assertFalse(transformed1.get());
        Assertions.assertTrue(transformed2.get());
        Assertions.assertFalse(transformed3.get());
    }

}
