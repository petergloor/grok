/*
 *    Copyright (C) 2016-2020 Grok Image Compression Inc.
 *
 *    This source code is free software: you can redistribute it and/or  modify
 *    it under the terms of the GNU Affero General Public License, version 3,
 *    as published by the Free Software Foundation.
 *
 *    This source code is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Affero General Public License for more details.
 *
 *    You should have received a copy of the GNU Affero General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 *    This source code incorporates work covered by the following copyright and
 *    permission notice:
 *
 * The copyright in this software is being made available under the 2-clauses
 * BSD License, included below. This software may be subject to other third
 * party and contributor rights, including patent rights, and no such rights
 * are granted under this license.
 *
 * Copyright (c) 2002-2014, Universite catholique de Louvain (UCL), Belgium
 * Copyright (c) 2002-2014, Professor Benoit Macq
 * Copyright (c) 2001-2003, David Janssens
 * Copyright (c) 2002-2003, Yannick Verschueren
 * Copyright (c) 2003-2007, Francois-Olivier Devaux
 * Copyright (c) 2003-2014, Antonin Descampe
 * Copyright (c) 2005, Herve Drolon, FreeImage Team
 * Copyright (c) 2008, 2011-2012, Centre National d'Etudes Spatiales (CNES), FR
 * Copyright (c) 2012, CS Systemes d'Information, France
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS `AS IS'
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#pragma once

#include <IBufferedStream.h>
#include "grk_config_private.h"
#include "IBitIO.h"

namespace grk {

#define GROK_STREAM_STATUS_OUTPUT  0x1U
#define GROK_STREAM_STATUS_INPUT   0x2U
#define GROK_STREAM_STATUS_END     0x4U
#define GROK_STREAM_STATUS_ERROR   0x8U

/**
 Byte input-output stream.
 */
struct BufferedStream: public IBufferedStream {

	BufferedStream(uint8_t *buffer, size_t buffer_size, bool l_is_input);
	~BufferedStream();

	/**
	 * user-supplied data
	 */
	void *m_user_data;

	/**
	 * Pointer to function to free m_user_data (nullptr at initialization)
	 * when destroying the stream. If pointer is nullptr the function is not
	 * called and the m_user_data is not freed (even if non-nullptr).
	 */
	grk_stream_free_user_data_fn m_free_user_data_fn;

	/**
	 * User data length.
	 * Currently set to size of file for file read stream, and size of buffer for buffer read/write stream
	 */
	uint64_t m_user_data_length;

	/**
	 * Pointer to actual read function (nullptr at initialization).
	 */
	grk_stream_read_fn m_read_fn;

	/**
	 * Pointer to actual zero copy read function (nullptr at initialization).
	 */
	grk_stream_zero_copy_read_fn m_zero_copy_read_fn;

	/**
	 * Pointer to actual write function (nullptr at initialization).
	 */
	grk_stream_write_fn m_write_fn;

	/**
	 * Pointer to actual seek function (if available).
	 */
	grk_stream_seek_fn m_seek_fn;

	/**
	 * Flags to tell the status of the stream.
	 * Used with GROK_STREAM_STATUS_* defines.
	 */
	uint32_t m_status;

	/**
	 * Reads some bytes from the stream.
	 * @param		p_buffer	pointer to the data buffer that will receive the data.
	 * @param		p_size		number of bytes to read.
	 
	 * @return		the number of bytes read, or -1 if an error occurred or if the stream is at the end.
	 */
	size_t read(uint8_t *p_buffer, size_t p_size);

	size_t read_data_zero_copy(uint8_t **p_buffer, size_t p_size);

	bool write_byte(uint8_t value);

	// write methods that take endian into account
	bool write_short(uint16_t value);
	bool write_24(uint32_t value);
	bool write_int(uint32_t value);
	bool write_64(uint64_t value);

	/**
	 * Writes some bytes to the stream (no correction for endian!).
	 * @param		p_buffer	pointer to the data buffer holds the data to be written.
	 * @param		p_size		number of bytes to write.
	 
	 * @return		the number of bytes written
	 */
	size_t write_bytes(const uint8_t *p_buffer, size_t p_size);

	/**
	 * Writes the content of the stream buffer to the stream.
	 
	 * @return		true if the data could be flushed, false else.
	 */
	bool flush();

	/**
	 * Skips a number of bytes from the stream.
	 * @param		p_size		the number of bytes to skip.
	 
	 * @return		true if success, or false if an error occurred.
	 */
	bool skip(int64_t p_size);

	/**
	 * Tells the byte offset on the stream (similar to ftell).
	 *
	 * @return		the current position of the stream.
	 */
	uint64_t tell(void);

	/**
	 * Get the number of bytes left before the end of the stream
	 *
	 * @return		Number of bytes left before the end of the stream.
	 */
	uint64_t get_number_byte_left(void);

	/**
	 * Seeks a number of bytes from the stream (absolute)
	 * @param		offset		the number of bytes to skip.
	 *
	 * @return		true if the stream is seekable.
	 */
	bool seek(uint64_t offset);

	/**
	 * Tells if the given stream is seekable.
	 */
	bool has_seek();

	bool supportsZeroCopy() {
		return isMemStream() && (m_status & GROK_STREAM_STATUS_INPUT);
	}
	uint8_t* getCurrentPtr() {
		return m_buf->curr_ptr();
	}

private:

	/**
	 * Skips a number of bytes from the stream.
	 * @param		p_size		the number of bytes to skip.
	 
	 * @return		true if success, or false if an error occurred.
	 */
	bool write_skip(int64_t p_size);

	/**
	 * Skips a number of bytes from the stream.
	 * @param		p_size		the number of bytes to skip.
	 
	 * @return		true if success, or false if an error occurred.
	 */
	bool read_skip(int64_t p_size);

	/**
	 * Absolute seek in stream.
	 * @param		offset		absolute offset
	 
	 * @return		true if success, or false if an error occurred.
	 */
	bool read_seek(uint64_t offset);

	/**
	 * Absolute seek in stream.
	 * @param		offset		absolute offset
	 * @return		true if success, or false if an error occurred.
	 */
	bool write_seek(uint64_t offset);

	void write_increment(size_t p_size);
	template<typename TYPE> bool write(TYPE value, uint8_t numBytes);
	void invalidate_buffer();

	bool isMemStream();

	///////////////////////////////////////////////////////////////////
	grk_buf *m_buf;

	// number of bytes read in, or slated for write
	size_t m_buffered_bytes;

	// number of seekable bytes in buffer. This will equal the number of bytes
	// read in the last media read.
	// We always have m_buffered_bytes <= m_read_bytes_seekable
	size_t m_read_bytes_seekable;

	// number of bytes read/written from the beginning of the stream
	uint64_t m_stream_offset;

};

template<typename TYPE> void grk_write(uint8_t *p_buffer, TYPE value,
		uint32_t nb_bytes) {
	if (nb_bytes == 0)
		return;
	assert(nb_bytes <= sizeof(TYPE));
#if defined(GROK_BIG_ENDIAN)
	const uint8_t * l_data_ptr = ((const uint8_t *)&value) + sizeof(TYPE) - nb_bytes;
	memcpy(p_buffer, l_data_ptr, nb_bytes);
#else
	const uint8_t *l_data_ptr = ((const uint8_t*) &value)
			+ (size_t)(nb_bytes - 1);
	for (uint32_t i = 0; i < nb_bytes; ++i) {
		*(p_buffer++) = *(l_data_ptr--);
	}
#endif
}

template<typename TYPE> void grk_write(uint8_t *p_buffer, TYPE value) {
	grk_write<TYPE>(p_buffer, value, sizeof(TYPE));
}

template<typename TYPE> void grk_read(const uint8_t *p_buffer, TYPE *value,
		uint32_t nb_bytes) {
	assert(nb_bytes > 0 && nb_bytes <= sizeof(TYPE));
#if defined(GROK_BIG_ENDIAN)
	auto l_data_ptr = ((uint8_t *)value);
	*value = 0;
	memcpy(l_data_ptr + sizeof(TYPE) - nb_bytes, p_buffer, nb_bytes);
#else
	auto l_data_ptr = ((uint8_t*) value) + nb_bytes - 1;
	*value = 0;
	for (uint32_t i = 0; i < nb_bytes; ++i)
		*(l_data_ptr--) = *(p_buffer++);
#endif
}

template<typename TYPE> void grk_read(const uint8_t *p_buffer, TYPE *value){
	grk_read<TYPE>(p_buffer, value, sizeof(TYPE));
}

}
