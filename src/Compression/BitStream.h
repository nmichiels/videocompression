#ifndef UTIL_BITSTREAM_H
#define UTIL_BITSTREAM_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


#ifndef _WIN32
	#define _WIN32 1
#endif

#ifndef WIN32
	#define WIN32 1
#endif


//#include <inttypes>
#include <cstdio>
#include <iostream>
using namespace std;

namespace util {

    class BitStream {
        public:
            BitStream(unsigned __int8 *b = 0, int s = 0, int p = 0, bool m = false) : buffer(b), size(s), position(p), managed(m) {}
			unsigned __int8* get_buffer() { return buffer; }
			const unsigned __int8* get_buffer() const { return buffer; }
			int get_size() const { return size; }
            void set_position(int p) { position = p; }
            int get_position() const { return position; }
			void reset() { set_position(0); }

        protected:
			unsigned __int8 *buffer;
            int size; // Size in bytes
			int position; // Position in bits
            bool managed;
    };

    class BitStreamWriter;

    /**
     * Class which eases reading bitwise from a buffer.
     */
	class BitStreamReader : public BitStream {
		public:
            /**
             * Create a bitstreamreader which reads from the provided buffer.
             *
             * @param [in] buffer The buffer from which bits will be read.
             * @param [in] size The size (expressed in bytes) of the buffer from which bits will be read.
             */
            BitStreamReader(unsigned __int8 *buffer, int size);

            /**
             * Create a bitstreamreader from an existing BitStreamWriter.
             *
             * @param b The existing BitStreamWriter.
             */
            BitStreamReader(BitStreamWriter &b);

			~BitStreamReader();

            /**
             * Read one bit from the bitstream.
             *
             * @return The value of the bit.
             */
            unsigned __int8 get_bit();

            /**
             * Get l bits from the bitstream
             *
             * @param [in] l number of bits to read
             * @return The value of the bits read
             *
             * buffer: 0101 1100, position==0
             * get(4) returns value 5, position==4
             */
            unsigned __int32 get(int l);

			void flush();
	};

    /**
     * Class which eases writing bitwise into a buffer.
     */
    class BitStreamWriter : public BitStream {
		public:
            /**
             * Create a bitstreamwriter which writes into the provided buffer.
             *
             * @param [in] buffer The buffer into which bits will be written.
             * @param [in] size The size (expressed in bytes) of the buffer into which bits will be written.
             */
            BitStreamWriter(unsigned __int8 *buffer, int size);

            /**
             * Create a bitstreamwriter and allocate a buffer for it.
             *
             * @param [in] size The size (expressed in bytes) of the buffer into which bits will be written.
             */
			BitStreamWriter(int s);

			~BitStreamWriter();

            /**
             * Write one bit into the bitstream.
             * @param [in] value The value to put into the bitstream.
             */
            void put_bit(unsigned __int8 value);

            /**
             * Put 'length' bits with value 'value' into the bitstream
             *
             * @param [in] length Number of bits to use for storing the value
             * @param [in] value The value to store
             *
             * buffer: xxxx xxxx, position==0
             * put(4, 5)
             * buffer: 1010 xxxx, position==4
             */
            void put(int length, unsigned __int32 value);

            /**
             * Byte-align
             */
            void flush();
	};

    /**
     * Write the contents of the bitstream to the specified file
     */
    void write(FILE *f, const BitStreamWriter &b);
}
#endif /* UTIL_BITSTREAM_H */

