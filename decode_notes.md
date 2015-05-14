For each label, if the data is encoded (`raw_format` is `4` or `6`), read `coded_data_size` bytes from the file, create a buffer of size `data_size` bytes, and pass both to the `decode` function.

Acquisitions are encoded individually. Each encoded acquisition is made up of a series of chunks. A chunk contains a header and corresponding data. The header is comprised of:

- 2-byte integer - decoded size in bytes of chunk data
- 2-byte integer - encoded size in bytes of chunk data
- 4-byte integer - destination of decoded data as an offset in bytes into the decoded acquisition buffer

The decoded size of a chunk appears to be a multiple of the number of samples for a single coil, etc, often just 1 line, but not always.
Example with `ky,kx = 178,480`:

    acq # | size  | # chunks | decoded chunk size | avg. encoded chunk size
    ------|-------|----------|--------------------|------------------------
    0     | 49920 | 26       | 1920               | 386.462
    1     | 49920 | 13       | 3840               | 766.154
    2     | 49920 | 26       | 1920               | 389.692
    3     | 49920 | 26       | 1920               | 391.692

A chunk is decoded by repeating the following process until every encoded integer in the chunk is decoded (`nelements = decoded_size / 4-bytes per integer`):

1. Decode 5-bit integer from buffer. This will be the bit-resolution.
1. Decode 5-bit integer from buffer. This will be the optional shift value (commonly zero)
1. Calculate an addend using the shift value: `(unsigned)(1 << shift) >> 1`, which is *essentially* `2 ^ (shift - 1)` but handles the case where `shift = -1`.
1. Repeat the following 16 times (or fewer if there are less than 16 encoded integers remaining):

    1. Decode an `n`-bit integer `x` from buffer, where `n = bit-resolution`.
    1. Left-shift `x` by `shift` and add the `addend`: `x = (x << shift) + addend`.
    1. Save `x` as a decoded 32-bit integer.

Decoding an `n`-bit integer from the buffer is achieved by keeping track of two values:

1. the "current" bit-position, and
2. a "current" 32-bit integer

The bit-position starts at zero for each chunk. The first decoding requires reading an entire 32-bit integer from the encoded buffer. This is saved as the "current" value and the bit-resolution is subtracted from the bit-position. The *next* decoding will either extract the next `n`-bit integer (`n = bit-resolution`) from the "current" value, or, if the bit-position is less than the bit-resolution, read another 32-bit integer from the encoded buffer, binary `or` it with the remaining "current" value, and update the "current" value and bit-position accordingly.
