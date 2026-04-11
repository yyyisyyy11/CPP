import java.util.Random;

/**
 * Dot product performance benchmark for multiple data types.
 * Single run per (type, n) to avoid JIT warm-up bias.
 * Output format: CSV lines "type,n,ms" for machine parsing.
 */
public class Dotproduct {

    // Volatile sink to prevent JIT from eliminating dot product calls
    static volatile double sink;

    // --- Dot product functions for each data type ---

    static float dotFloat(float[] a, float[] b) {
        float sum = 0.0f;
        for (int i = 0; i < a.length; i++) {
            sum += a[i] * b[i];
        }
        return sum;
    }

    static double dotDouble(double[] a, double[] b) {
        double sum = 0.0;
        for (int i = 0; i < a.length; i++) {
            sum += a[i] * b[i];
        }
        return sum;
    }

    static long dotInt(int[] a, int[] b) {
        long sum = 0;
        for (int i = 0; i < a.length; i++) {
            sum += (long) a[i] * b[i];
        }
        return sum;
    }

    static long dotShort(short[] a, short[] b) {
        long sum = 0;
        for (int i = 0; i < a.length; i++) {
            sum += (long) a[i] * b[i];
        }
        return sum;
    }

    static long dotByte(byte[] a, byte[] b) {
        long sum = 0;
        for (int i = 0; i < a.length; i++) {
            sum += (long) a[i] * b[i];
        }
        return sum;
    }

    // --- Benchmark runners (single run) ---

    static void benchmarkFloat(int n, Random rand) {
        float[] a = new float[n];
        float[] b = new float[n];
        for (int i = 0; i < n; i++) {
            a[i] = rand.nextFloat();
            b[i] = rand.nextFloat();
        }

        long startTime = System.nanoTime();
        float result = dotFloat(a, b);
        long endTime = System.nanoTime();
        sink = result;
        System.out.printf("float,%d,%.6f%n", n, (endTime - startTime) / 1e6);
    }

    static void benchmarkDouble(int n, Random rand) {
        double[] a = new double[n];
        double[] b = new double[n];
        for (int i = 0; i < n; i++) {
            a[i] = rand.nextDouble();
            b[i] = rand.nextDouble();
        }

        long startTime = System.nanoTime();
        double result = dotDouble(a, b);
        long endTime = System.nanoTime();
        sink = result;
        System.out.printf("double,%d,%.6f%n", n, (endTime - startTime) / 1e6);
    }

    static void benchmarkInt(int n, Random rand) {
        int[] a = new int[n];
        int[] b = new int[n];
        for (int i = 0; i < n; i++) {
            a[i] = rand.nextInt(201) - 100;
            b[i] = rand.nextInt(201) - 100;
        }

        long startTime = System.nanoTime();
        long result = dotInt(a, b);
        long endTime = System.nanoTime();
        sink = result;
        System.out.printf("int,%d,%.6f%n", n, (endTime - startTime) / 1e6);
    }

    static void benchmarkShort(int n, Random rand) {
        short[] a = new short[n];
        short[] b = new short[n];
        for (int i = 0; i < n; i++) {
            a[i] = (short) (rand.nextInt(201) - 100);
            b[i] = (short) (rand.nextInt(201) - 100);
        }

        long startTime = System.nanoTime();
        long result = dotShort(a, b);
        long endTime = System.nanoTime();
        sink = result;
        System.out.printf("short,%d,%.6f%n", n, (endTime - startTime) / 1e6);
    }

    static void benchmarkByte(int n, Random rand) {
        byte[] a = new byte[n];
        byte[] b = new byte[n];
        for (int i = 0; i < n; i++) {
            a[i] = (byte) (rand.nextInt(201) - 100);
            b[i] = (byte) (rand.nextInt(201) - 100);
        }

        long startTime = System.nanoTime();
        long result = dotByte(a, b);
        long endTime = System.nanoTime();
        sink = result;
        System.out.printf("byte,%d,%.6f%n", n, (endTime - startTime) / 1e6);
    }

    // --- Main ---

    public static void main(String[] args) {
        int[] sizes = { 128, 1000, 10000, 100000, 1000000, 10000000, 100000000 };
        Random rand = new Random(42); // Fixed seed for reproducibility

        // Output CSV header
        System.out.println("type,n,ms");

        System.err.println("Data type: float");
        for (int n : sizes)
            benchmarkFloat(n, rand);

        System.err.println("Data type: double");
        for (int n : sizes)
            benchmarkDouble(n, rand);

        System.err.println("Data type: int");
        for (int n : sizes)
            benchmarkInt(n, rand);

        System.err.println("Data type: short");
        for (int n : sizes)
            benchmarkShort(n, rand);

        System.err.println("Data type: byte");
        for (int n : sizes)
            benchmarkByte(n, rand);
    }
}