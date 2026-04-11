import java.util.Random;

/**
 * JIT Warmup Verification: fixed n, repeated runs, observe speed changes.
 * This demonstrates that JIT compilation is triggered by execution count,
 * not by vector size or wall-clock time.
 */
public class JitWarmup {

    static float dotFloat(float[] a, float[] b) {
        float sum = 0.0f;
        for (int i = 0; i < a.length; i++) {
            sum += a[i] * b[i];
        }
        return sum;
    }

    public static void main(String[] args) {
        final int n = 10000; // fixed, relatively small n
        final int rounds = 30; // run 30 times to observe JIT kicking in

        Random rand = new Random(42);
        float[] a = new float[n];
        float[] b = new float[n];
        for (int i = 0; i < n; i++) {
            a[i] = rand.nextFloat();
            b[i] = rand.nextFloat();
        }

        System.out.println("=== JIT Warmup Verification ===");
        System.out.println("Fixed n = " + n + ", running " + rounds + " rounds\n");
        System.out.printf("  %-8s  %12s  %s%n", "Round", "Time (ms)", "Note");
        System.out.println("  " + "-".repeat(50));

        for (int r = 1; r <= rounds; r++) {
            long start = System.nanoTime();
            float result = dotFloat(a, b);
            long end = System.nanoTime();

            double timeMs = (end - start) / 1e6;

            // Detect significant speedup
            String note = "";
            if (r == 1)
                note = "<-- first call (interpreted)";

            System.out.printf("  %-8d  %12.4f  %s%n", r, timeMs, note);
        }

        System.out.println("\n  " + "-".repeat(50));
        System.out.println("  If JIT works, you should see a sudden speed drop");
        System.out.println("  after a few rounds as the method gets compiled.");
    }
}
