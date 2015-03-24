import java.math.BigInteger;
import java.nio.ByteBuffer;
import java.nio.charset.Charset;

public class MurmurHash {
  private static final Charset UTF8 = Charset.forName("UTF-8");

  /**
   * Hashes bytes in an array.
   * @param data The bytes to hash.
   * @param seed The seed for the hash.
   * @return The 32 bit hash of the bytes in question.
   */
  public static int MurmurHash3_x86_32(byte[] data, int seed) {
    final int c1 = 0xcc9e2d51;
    final int c2 = 0x1b873593;
    final int length = data.length;
    final int length4 = length >>> 2;

    int h1 = seed;

    // body
    for (int i = 0; i < length4; ++i) {
      final int i4 = i << 2;
      int k1 = (data[i4] & 0xFF) + ((data[i4 + 1] & 0xFF) << 8)
          + ((data[i4 + 2] & 0xFF) << 16) + ((data[i4 + 3] & 0xFF) << 24);

      k1 *= c1;
      k1 = ((k1 << 15) | (k1 >>> 17));
      k1 *= c2;

      h1 ^= k1;
      h1 = ((h1 << 13) | (h1 >>> 19));
      h1 = h1 * 5 + 0xe6546b64;
    }

    // tail
    int k1 = 0;

    final int tail = length4 << 2;
    switch (length & 3) {
    case 3:
      k1 ^= data[tail + 2] << 16;
    case 2:
      k1 ^= data[tail + 1] << 8;
    case 1:
      k1 ^= data[tail];
      k1 *= c1;
      k1 = ((k1 << 15) | (k1 >>> 17));
      k1 *= c2;
      h1 ^= k1;
    }

    // finalization
    h1 ^= length;
    // fmix32(h1)
    h1 ^= h1 >>> 16;
    h1 *= 0x85ebca6b;
    h1 ^= h1 >>> 13;
    h1 *= 0xc2b2ae35;
    h1 ^= h1 >>> 16;

    return h1;
  }

  public static long MurmurHash64(final byte[] data, long seed) {
    final long M = 0xc6a4a7935bd1e995L;
    final int R = 47;
    final int length = data.length;
    final int length8 = length >>> 3;
    long h = (seed & 0xffffffffL) ^ (length * M);

    for (int i = 0; i < length8; i++) {
      final int i8 = i << 3;
      long k = (data[i8 + 0] & 0xFFL) + ((data[i8 + 1] & 0xFFL) << 8)
          + ((data[i8 + 2] & 0xFFL) << 16) + ((data[i8 + 3] & 0xFFL) << 24)
          + ((data[i8 + 4] & 0xFFL) << 32) + ((data[i8 + 5] & 0xFFL) << 40)
          + ((data[i8 + 6] & 0xFFL) << 48) + ((data[i8 + 7] & 0xFFL) << 56);

      k *= M;
      k ^= k >>> R;
      k *= M;

      h ^= k;
      h *= M;
    }

    final int tail = length8 << 3;
    switch (length & 7) {
    case 7:
      h ^= (data[tail + 6] & 0xFFL) << 48;
    case 6:
      h ^= (data[tail + 5] & 0xFFL) << 40;
    case 5:
      h ^= (data[tail + 4] & 0xFFL) << 32;
    case 4:
      h ^= (data[tail + 3] & 0xFFL) << 24;
    case 3:
      h ^= (data[tail + 2] & 0xFFL) << 16;
    case 2:
      h ^= (data[tail + 1] & 0xFFL) << 8;
    case 1:
      h ^= (data[tail] & 0xFFL);
      h *= M;
    }

    h ^= h >>> R;
    h *= M;
    h ^= h >>> R;

    return h;
  }

  private static long getblock(byte[] key, int i) {
    return ((key[i + 0] & 0xFFL) << 0) | ((key[i + 1] & 0xFFL) << 8)
        | ((key[i + 2] & 0xFFL) << 16) | ((key[i + 3] & 0xFFL) << 24)
        | ((key[i + 4] & 0xFFL) << 32) | ((key[i + 5] & 0xFFL) << 40)
        | ((key[i + 6] & 0xFFL) << 48) | ((key[i + 7] & 0xFFL) << 56);
  }

  private static long fmix(long k) {
    k ^= k >>> 33;
    k *= 0xff51afd7ed558ccdL;
    k ^= k >>> 33;
    k *= 0xc4ceb9fe1a85ec53L;
    k ^= k >>> 33;

    return k;
  }

  public static long[] MurmurHash3_x64_128(final byte[] key, final int seed) {
    final long c1 = 0x87c37b91114253d5L;
    final long c2 = 0x4cf5ad432745937fL;
    final int length = key.length, length16 = length >>> 4;
    long h1 = seed;
    long h2 = seed;
    long k1, k2;

    for (int i = 0; i < length16; i++) {
      k1 = getblock(key, i * 2 * 8);
      k2 = getblock(key, (i * 2 + 1) * 8);

      k1 *= c1;
      k1 = (k1 << 31) | (k1 >>> (64 - 31));
      k1 *= c2;
      h1 ^= k1;

      h1 = (h1 << 27) | (h1 >>> (64 - 27));
      h1 += h2;
      h1 = h1 * 5 + 0x52dce729;

      k2 *= c2;
      k2 = (k2 << 33) | (k2 >>> (64 - 33));
      k2 *= c1;
      h2 ^= k2;

      h2 = (h2 << 31) | (h2 >>> (64 - 31));
      h2 += h1;
      h2 = h2 * 5 + 0x38495ab5;
    }

    k1 = 0;
    k2 = 0;

    final int tail = length16 << 4;

    switch (length & 15) {
    case 15:
      k2 ^= (long) key[tail + 14] << 48;
    case 14:
      k2 ^= (long) key[tail + 13] << 40;
    case 13:
      k2 ^= (long) key[tail + 12] << 32;
    case 12:
      k2 ^= (long) key[tail + 11] << 24;
    case 11:
      k2 ^= (long) key[tail + 10] << 16;
    case 10:
      k2 ^= (long) key[tail + 9] << 8;
    case 9:
      k2 ^= (long) key[tail + 8] << 0;
      k2 *= c2;
      k2 = (k2 << 33) | (k2 >>> (64 - 33));
      k2 *= c1;
      h2 ^= k2;
    case 8:
      k1 ^= (long) key[tail + 7] << 56;
    case 7:
      k1 ^= (long) key[tail + 6] << 48;
    case 6:
      k1 ^= (long) key[tail + 5] << 40;
    case 5:
      k1 ^= (long) key[tail + 4] << 32;
    case 4:
      k1 ^= (long) key[tail + 3] << 24;
    case 3:
      k1 ^= (long) key[tail + 2] << 16;
    case 2:
      k1 ^= (long) key[tail + 1] << 8;
    case 1:
      k1 ^= (long) key[tail + 0] << 0;
      k1 *= c1;
      k1 = (k1 << 31) | (k1 >>> (64 - 31));
      k1 *= c2;
      h1 ^= k1;
    }

    h1 ^= length;
    h2 ^= length;

    h1 += h2;
    h2 += h1;

    h1 = fmix(h1);
    h2 = fmix(h2);

    h1 += h2;
    h2 += h1;

    return new long[]{h1, h2};
  }

  public static long MurmurHash3_x64_128(String str) {
    return MurmurHash3_x64_128(str.getBytes(UTF8), str.length())[0];
  }

  public static String ToUnsignedLongString(long signedLong) {
    return new BigInteger(ByteBuffer.allocate(9).put((byte) 0).putLong(1, signedLong).array()).toString();
  }

  public static void main(String[] args) {
    for (String str : args) {
      System.out.println(str + "\t" + MurmurHash3_x64_128(str.getBytes(), 100003)[0]);
    }
  }
}
