// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file hal/io.h
 */
#if !defined(IO_H)
  #define IO_H

  #include <stdbool.h>
  #include <stdint.h>

  /**
   * Abstracted file path.
   * All file operations use an abstracted path so they can be stored in the
   * appropriate location dependent on the platform.
   */
  typedef enum {
    ioPathSaveFile = 0, ///< save file used in SAVE and LOAD functions
    ioPathPgmFile  = 1, ///< program file
    ioPathTestPgms = 2, ///< test programs
    ioPathBackup   = 3  ///< backup file for full state used in simulators
  } ioFilePath_t;

  /**
   * File open mode.
   * Files must be opened with a mode to indicate whether they will be read, or
   * written to, or both. All operations are in binary modes where the platform
   * allows this to be specified.
   */
  typedef enum {
    ioModeRead   = 0, ///< open the file in read-only mode
    ioModeWrite  = 1, ///< open the file in write-only mode
    ioModeUpdate = 2  ///< open the file in read/write mode
  } ioFileMode_t;

  /**
   * Open a file.
   * File operations require a file to be opened first. Opening the file returns
   * true if the file was opened and false otherwise. The HAL only allows a single
   * open file at any one time and this should be closed with ::ioFileClose as
   * soon as possible.
   *
   * \param[in] path the enumeration value for the particular file to open
   * \param[in] mode the mode to open the file (read, write, update)
   * \return true if file opened successfully
   */
  bool ioFileOpen(ioFilePath_t path, ioFileMode_t mode);

  /**
   * Write to the open file.
   *
   * \param[in] buffer the binary stream to write
   * \param[in] size how many bytes to write
   */
  void ioFileWrite(const void *buffer, uint32_t size);

  /**
   * Read from the open file.
   * The buffer must have an allocated size at least as long as the specified
   * size.
   *
   * \param[out] buffer the allocated buffer to read into
   * \param[in] size how many bytes to read
   * \return how many bytes were actually read
   */
  uint32_t ioFileRead(void *buffer, uint32_t size);

  /**
   * Move to a particular position in the file.
   * This is an absolute position from the beginning of the file.
   *
   * \param[in] position position to move to
   */
  void ioFileSeek(uint32_t position);

  /**
   * Close the open file.
   * Files must be closed to avoid resource leaks.
   */
  void ioFileClose(void);

#endif // IO_H
