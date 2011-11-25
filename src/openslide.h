/*
 *  OpenSlide, a library for reading whole slide image files
 *
 *  Copyright (c) 2007-2010 Carnegie Mellon University
 *  All rights reserved.
 *
 *  OpenSlide is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as
 *  published by the Free Software Foundation, version 2.1.
 *
 *  OpenSlide is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with OpenSlide. If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

/**
 * @file openslide.h
 * The API for the OpenSlide library.
 */

#ifndef OPENSLIDE_OPENSLIDE_H_
#define OPENSLIDE_OPENSLIDE_H_

#include <openslide-features.h>

#include <stdio.h>
#include <stdint.h>

/**
 * The main OpenSlide type.
 */
typedef struct _openslide openslide_t;


/**
 * @name Basic Usage
 * Opening, reading, and closing. Note that the terminology used by
 * OpenSlide is slightly different from DICOM and others: notably,
 * a DICOM "level" is known as a "layer" in OpenSlide.
 */
//@{

/**
 * Do a quick check to see if a whole slide image is valid.
 *
 * @param filename The filename to check.
 * @return If openslide_open() will succeed.
 */
OPENSLIDE_PUBLIC()
bool openslide_can_open(const char *filename);


/**
 * Open a whole slide image.
 *
 * @param filename The filename to open.
 * @return A new OpenSlide object, or NULL if an error occurred.
 */
OPENSLIDE_PUBLIC()
openslide_t *openslide_open(const char *filename);


/**
 * Get the number of layers in the whole slide image.
 *
 * @param osr The OpenSlide object.
 * @return The number of layers, or -1 if an error occurred.
 */
OPENSLIDE_PUBLIC()
int32_t openslide_get_layer_count(openslide_t *osr);


/**
 * Get the dimensions of layer 0 (the largest layer). Exactly
 * equivalent to calling openslide_get_layer_dimensions(osr, 0, w, h).
 *
 * @param osr The OpenSlide object.
 * @param[out] w The width of the image, or -1 if an error occurred.
 * @param[out] h The height of the image, or -1 if an error occurred.
 */
OPENSLIDE_PUBLIC()
void openslide_get_layer0_dimensions(openslide_t *osr, int64_t *w, int64_t *h);


/**
 * Get the dimensions of a layer.
 *
 * @param osr The OpenSlide object.
 * @param layer The desired layer.
 * @param[out] w The width of the image, or -1 if an error occurred
 *               or the layer was out of range.
 * @param[out] h The height of the image, or -1 if an error occurred
 *               or the layer was out of range.
 */
OPENSLIDE_PUBLIC()
void openslide_get_layer_dimensions(openslide_t *osr, int32_t layer,
				    int64_t *w, int64_t *h);


/**
 * Get the downsampling factor of a given layer.
 *
 * @param osr The OpenSlide object.
 * @param layer The desired layer.
 * @return The downsampling factor for this layer, or -1.0 if an error occurred
 *         or the layer was out of range.
 */
OPENSLIDE_PUBLIC()
double openslide_get_layer_downsample(openslide_t *osr, int32_t layer);


/**
 * Get the best layer to use for displaying the given downsample.
 *
 * @param osr The OpenSlide object.
 * @param downsample The downsample factor.
 * @return The layer identifier, or -1 if an error occurred.
 */
OPENSLIDE_PUBLIC()
int32_t openslide_get_best_layer_for_downsample(openslide_t *osr,
						double downsample);

/**
 * Copy pre-multiplied ARGB data from a whole slide image.
 *
 * This function reads and decompresses a region of a whole slide
 * image into the specified memory location. @p dest must be a valid
 * pointer to enough memory to hold the region, at least (@p w * @p h * 4)
 * bytes in length. If an error occurs or has occurred, then the memory
 * pointed to by @p dest will be cleared.
 *
 * @param osr The OpenSlide object.
 * @param dest The destination buffer for the ARGB data.
 * @param x The top left x-coordinate, in the layer 0 reference frame.
 * @param y The top left y-coordinate, in the layer 0 reference frame.
 * @param layer The desired layer.
 * @param w The width of the region. Must be non-negative.
 * @param h The height of the region. Must be non-negative.
 */
OPENSLIDE_PUBLIC()
void openslide_read_region(openslide_t *osr,
			   uint32_t *dest,
			   int64_t x, int64_t y,
			   int32_t layer,
			   int64_t w, int64_t h);


/**
 * Close an OpenSlide object. After this call returns, the object cannot
 * be used anymore.
 *
 * @param osr The OpenSlide object.
 */
OPENSLIDE_PUBLIC()
void openslide_close(openslide_t *osr);
//@}

/**
 * @name Error Handling
 * A simple mechanism for detecting errors.
 *
 * Sometimes an unrecoverable error can occur that will invalidate the
 * OpenSlide object. (This is typically something like an I/O error or
 * data corruption.)  When such an error happens in an OpenSlide
 * object, the object will move terminally into an error state.
 *
 * While an object is in an error state, no OpenSlide functions will
 * have any effect on it except for openslide_close(). Functions
 * that are expected to return values will instead return an error
 * value, typically something like NULL or -1. openslide_read_region()
 * will clear its destination buffer instead of painting into
 * it. openslide_get_error() will return a non-NULL string containing
 * an error message. See the documentation for each function for
 * details on what is returned in case of error.
 *
 * This style of error handling allows programs written in C to check
 * for errors only when convenient, because the error state is
 * terminal and the OpenSlide functions return harmlessly when there
 * has been an error.
 *
 * If writing wrappers for OpenSlide in languages that support
 * exceptions, it is recommended that the error state be checked after
 * each call and converted into an exception for that language.
 */
//@{
/**
 * Get the current error string.
 *
 * For a given OpenSlide object, once this function returns a non-NULL
 * value, the only useful operation on the object is to call
 * openslide_close() to free its resources.
 *
 * @param osr The OpenSlide object.
 * @return A string describing the original error that caused
 * the problem, or NULL if no error has occurred.
 *
 */
OPENSLIDE_PUBLIC()
const char *openslide_get_error(openslide_t *osr);
//@}

/**
 * @name Predefined Properties
 * Some predefined properties.
 */
//@{

/**
 * The name of the property containing a slide's comment, if any.
 */
#define OPENSLIDE_PROPERTY_NAME_COMMENT "openslide.comment"

/**
 * The name of the property containing an identification of the vendor.
 */
#define OPENSLIDE_PROPERTY_NAME_VENDOR "openslide.vendor"

/**
 * The name of the property containing the "quickhash-1" sum.
 */
#define OPENSLIDE_PROPERTY_NAME_QUICKHASH1 "openslide.quickhash-1"

/**
 * The name of the property containing the quickhash-1 calculation time.
 */
#define OPENSLIDE_PROPERTY_NAME_QUICKHASH1_TIME "openslide.quickhash-1-time"

/**
 * The name of the property containing the openslide_open time.
 */
#define OPENSLIDE_PROPERTY_NAME_OPEN_TIME "openslide.open-time"

/**
 * The name of the property containing a slide's background color, if any.
 * It is represented as an RGB hex triplet.
 */
#define OPENSLIDE_PROPERTY_NAME_BACKGROUND_COLOR "openslide.background-color"
//@}

/**
 * @name Properties
 * Querying properties.
 */
//@{

/**
 * Get the NULL-terminated array of property names.
 *
 * Certain vendor-specific metadata properties may exist
 * within a whole slide image. They are encoded as key-value
 * pairs. This call provides a list of names as strings
 * that can be used to read properties with openslide_get_property_value().
 *
 * @param osr The OpenSlide object.
 * @return A NULL-terminated string array of property names, or
 *         an empty array if an error occurred.
 */
OPENSLIDE_PUBLIC()
const char * const *openslide_get_property_names(openslide_t *osr);


/**
 * Get the value of a single property.
 *
 * Certain vendor-specific metadata properties may exist
 * within a whole slide image. They are encoded as key-value
 * pairs. This call provides the value of the property given
 * by @p name.
 *
 * @param osr The OpenSlide object.
 * @param name The name of the desired property. Must be
               a valid name as given by openslide_get_property_names().
 * @return The value of the named property, or NULL if the property
 *         doesn't exist or an error occurred.
 */
OPENSLIDE_PUBLIC()
const char *openslide_get_property_value(openslide_t *osr, const char *name);

/**
 * Get the comment (if any) for this image. Exactly equivalent to calling
 * openslide_get_property_value() with #OPENSLIDE_PROPERTY_NAME_COMMENT.
 *
 * @param osr The OpenSlide object.
 * @return The comment for this image, or NULL if an error occurred.
 */
OPENSLIDE_PUBLIC()
const char *openslide_get_comment(openslide_t *osr);

//@}

/**
 * @name Associated Images
 * Reading associated images.
 */
//@{

/**
 * Get the NULL-terminated array of associated image names.
 *
 * Certain vendor-specific associated images may exist
 * within a whole slide image. They are encoded as key-value
 * pairs. This call provides a list of names as strings
 * that can be used to read associated images with
 * openslide_get_associated_image_dimensions() and
 * openslide_read_associated_image().
 *
 * @param osr The OpenSlide object.
 * @return A NULL-terminated string array of associated image names, or
           an empty array if an error occurred.
 */
OPENSLIDE_PUBLIC()
const char * const *openslide_get_associated_image_names(openslide_t *osr);

/**
 * Get the dimensions of an associated image.
 *
 * This function returns the width and height of an associated image
 * associated with a whole slide image. Once the dimensions are known,
 * use openslide_read_associated_image() to read the image.
 *
 * @param osr The OpenSlide object.
 * @param name The name of the desired associated image. Must be
 *            a valid name as given by openslide_get_associated_image_names().
 * @param[out] w The width of the associated image, or -1 if an error occurred.
 * @param[out] h The height of the associated image, or -1 if an error occurred.
 */
OPENSLIDE_PUBLIC()
void openslide_get_associated_image_dimensions(openslide_t *osr,
					       const char *name,
					       int64_t *w, int64_t *h);


/**
 * Copy pre-multiplied ARGB data from an associated image.
 *
 * This function reads and decompresses an associated image associated
 * with a whole slide image. @p dest must be a valid pointer to enough
 * memory to hold the image, at least (width * height * 4) bytes in
 * length.  Get the width and height with
 * openslide_get_associated_image_dimensions(). This call does nothing
 * if an error occurred.
 *
 * @param osr The OpenSlide object.
 * @param dest The destination buffer for the ARGB data.
 * @param name The name of the desired associated image. Must be
 *             a valid name as given by openslide_get_associated_image_names().
 */
OPENSLIDE_PUBLIC()
void openslide_read_associated_image(openslide_t *osr,
				     const char *name,
				     uint32_t *dest);
//@}


// these are meant to throw compile- and link-time errors,
// since the functions they replace were never implemented
int _openslide_give_prefetch_hint_UNIMPLEMENTED(void);
void _openslide_cancel_prefetch_hint_UNIMPLEMENTED(void);
#define openslide_give_prefetch_hint(osr, x, y, layer, w, h)	\
  _openslide_give_prefetch_hint_UNIMPLEMENTED(-1);
#define openslide_cancel_prefetch_hint(osr, prefetch_id)	\
  _openslide_cancel_prefetch_hint_UNIMPLEMENTED(-1)


/**
 * @mainpage OpenSlide
 *
 * OpenSlide is a C library that provides a simple interface to read
 * whole-slide images (also known as virtual slides). See
 * http://openslide.org/ for more details.
 */

#endif
