/**
 * This work copyright Chao Sun(qq:296449610) and licensed under
 * a Creative Commons Attribution 3.0 Unported License(https://creativecommons.org/licenses/by/3.0/).
 */

#ifndef CCRAFT_ERRORS_H
#define CCRAFT_ERRORS_H

#include <string>
#include <exception>
#include <sstream>

using std::string;
using std::stringstream;

static const string NotSupportError = "Not support!";
static const string ParamNullptrError = "Param cannot be null!";
static const string ParamOutOfRangeError = "Param is out of range!";
static const string CreateDirError = "Cannot create dir!";

#define THROW_ERR_BASE(errstr)              stringstream ss;                                                                     \
                                            ss << __FILE__ << ":" << __LINE__ << ":" << __FUNCTION__ << ": " << errstr.c_str();

#define THROW_RUNTIME_ERR(errstr)           THROW_ERR_BASE(errstr);                                                              \
                                            throw std::runtime_error(ss.str());

#define THROW_INVALID_ARG_ERR(errstr)       THROW_ERR_BASE(errstr);                                                              \
                                            throw std::invalid_argument(ss.str());

#define THROW_NOT_SUPPORT_ERR()             THROW_RUNTIME_ERR(NotSupportError)
#define THROW_PARAM_NULL_ERR()              THROW_INVALID_ARG_ERR(ParamNullptrError)
#define THROW_PARAM_OUT_RANGE_ERR()         THROW_INVALID_ARG_ERR(ParamOutOfRangeError)
#define THROW_CREATE_DIR_ERR()              THROW_RUNTIME_ERR(CreateDirError)

#endif //CCRAFT_ERRORS_H
