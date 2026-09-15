
#ifndef SOLARIS_PORT_H
#define SOLARIS_PORT_H

/**
 * @file solaris-port.h
 * @brief Solaris event ports compatibility layer for MSVC.
 */

/* clang-format off */
#include <stddef.h>
/* clang-format on */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Error codes returned by solaris-port functions.
 */
enum solaris_port_error_code {
  /** @brief Operation completed successfully. */
  SOLARIS_PORT_SUCCESS = 0,
  /** @brief A null pointer was passed as an argument. */
  SOLARIS_PORT_ERROR_NULL_POINTER = 1,
  /** @brief The requested operation is unsupported on this platform. */
  SOLARIS_PORT_ERROR_UNSUPPORTED = 2
};

/**
 * @brief Retrieves information on solaris-port availability.
 * @param[out] out_available Pointer to integer receiving availability status (0
 * for stub).
 * @return SOLARIS_PORT_SUCCESS on success, or SOLARIS_PORT_ERROR_NULL_POINTER
 * on NULL pointer.
 */
enum solaris_port_error_code solaris_port_get_info(int *out_available);

#if defined(_MSC_VER) || defined(_WIN32)

/**
 * @struct port_event
 * @brief Solaris port event notification structure.
 */
struct port_event {
  /** @brief Event types that occurred. */
  int portev_events;
  /** @brief Event source. */
  unsigned short portev_source;
  /** @brief Padding. */
  unsigned short portev_pad;
  /** @brief Event object pointer or identifier. */
  unsigned int portev_object;
  /** @brief User pointer associated with object. */
  void *portev_user;
};

/**
 * @brief Forward declaration of struct timespec.
 */
struct timespec;

/**
 * @brief Creates a new event port.
 * @return File descriptor for the new port, or -1 on failure with errno set.
 */
int port_create(void);

/**
 * @brief Associates a specific event source and object with an event port.
 * @param port Port descriptor.
 * @param source Event source type.
 * @param object Object identifier.
 * @param events Monitored events mask.
 * @param user User data associated with the association.
 * @return 0 on success, or -1 on error with errno set.
 */
int port_associate(int port, int source, unsigned int object, int events,
                   void *user);

/**
 * @brief Retrieves one or more events from an event port.
 * @param port Port descriptor.
 * @param list Pointer to an array of port_event structures.
 * @param max Maximum number of events to retrieve.
 * @param nget In/out pointer specifying desired count and receiving retrieved
 * count.
 * @param timeout Maximum duration to wait.
 * @return 0 on success, or -1 on error with errno set.
 */
int port_getn(int port, struct port_event *list, unsigned int max,
              unsigned int *nget, const struct timespec *timeout);

#endif /* defined(_MSC_VER) || defined(_WIN32) */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SOLARIS_PORT_H */
