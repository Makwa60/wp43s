// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: Copyright The WP43 Authors

/**
 * \file hal/io.h
 */
#if !defined(IO_H)
  #define IO_H

  #include <stdbool.h>
  #include <stdint.h>

// State files dir
#define STATE_DIR      "/STATE"
#define STATE_EXT      ".s43"
// --------------------------------

// Program files dir
#define PROGRAMS_DIR   "/PROGRAMS"
#define PRGM_EXT       ".p43"
// --------------------------------

#define MRET_SAVESTATE   777
#define MRET_LOADSTATE   888

// --------------------------------

  /**
   * Abstracted file path.
   * All file operations use an abstracted path so they can be stored in the
   * appropriate location dependent on the platform.
   */
  typedef enum {
    ioPathSaveFile      = 0, ///< save file used in SAVE and LOAD functions
    ioPathPgmFile       = 1, ///< program file
    ioPathTestPgms      = 2, ///< test programs
    ioPathBackup        = 3, ///< backup file for full state used in simulators
    ioPathRegDump       = 4, ///< register dump file which enables to view the full digits of long integers
    ioPathSaveStateFile = 6, ///< state file used in SAVEST function
    ioPathLoadStateFile = 7, ///< state file used in LOADST function
    ioPathSaveProgram   = 8, ///< program file used in WRITEP function
    ioPathLoadProgram   = 9, ///< program file used in READP function
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

  /**
   * Delete the given file.
   * The file should not be open.
   *
   * \param[in] path file to delete
   * \param[out] errorNumber error code given by the platform if there's an error
   * \return true if delete succeeded
   */
  bool ioFileRemove(ioFilePath_t path, uint32_t *errorNumber);
  
   /**
   * Callback function for Save State File selected file.
   * Called from the DMCP file_selection_screen() dialog.
   *
   * \param[in] path file selected
   * \param[in] file name selected
   * \param[in] data - unsused 
   * \param[out] set tmpFileName with the path file selected
   * \return MRET_SAVESTATE
   */
  int save_statefile(const char * fpath, const char * fname, void * data);
  
   /**
   * Callback function for Load State File selected file.
   * Called from the DMCP file_selection_screen() dialog.
   *
   * \param[in] path file selected
   * \param[in] file name selected
   * \param[in] data - unsused 
   * \param[out] set tmpFileName with the path file selected
   * \return MRET_LOADSTATE
   */
  int load_statefile(const char * fpath, const char * fname, void * data);
  
   /**
   * Callback function for Save Program File selected file.
   * Called from the DMCP file_selection_screen() dialog.
   *
   * \param[in] path file selected
   * \param[in] file name selected
   * \param[in] data - unsused 
   * \param[out] set tmpFileName with the path file selected
   * \return MRET_SAVESTATE
   */
  int save_programfile(const char * fpath, const char * fname, void * data);
  
   /**
   * Callback function for Load Program File selected file.
   * Called from the DMCP file_selection_screen() dialog.
   *
   * \param[in] path file selected
   * \param[in] file name selected
   * \param[in] data - unsused 
   * \param[out] set tmpFileName with the path file selected
   * \return MRET_LOADSTATE
   */
  int load_programfile(const char * fpath, const char * fname, void * data);


#endif // IO_H
