package net.sourceforge.resample;

public class Resample {
    private static final String RESAMPLE_LIB = "resample";
    
    static {
        System.loadLibrary(RESAMPLE_LIB);
    }
    
    public static native void downsample(short outputBuffer[], short inputLeft[], short inputRight[]);
    
    public static native void initialize(int inputRate, int outputRate, int channels);
    
    public static native int process(short inputBuffer[], short outputBuffer[], boolean isLast);
    
    public static native void close();
}