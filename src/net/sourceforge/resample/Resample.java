package net.sourceforge.resample;

public class Resample {

    public static final int DEFAULT_BUFFER_SIZE = 4096;
    public static final int MAX_CHANNELS = 2;
    public static final int CHANNEL_MONO = 0;
    public static final int CHANNEL_LEFT = 0;
    public static final int CHANNEL_RIGHT = 1;

    private static final String RESAMPLE_LIB = "resample";

    static {
        System.loadLibrary(RESAMPLE_LIB);
    }

    public static native void downmix(short outputBuffer[], short inputLeft[], short inputRight[], int numSamples);

    public static native int resample(double factor, short inputBuffer[], short outputBuffer[], int numSamples);

    public static native void initialize(int inputRate, int outputRate, int bufferSize, int channels);

    public static native double getFactor();

    public static native int process(short inputBuffer[], short outputBuffer[], int channel, boolean isLast);

    public static native void close();
}