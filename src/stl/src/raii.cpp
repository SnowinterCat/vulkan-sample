#include "vks/stl/deleter.hpp"

VKS_BEGIN
VKS_STL_BEGIN

void FileDeleter::operator()(FILE *file)
{
    if (file != nullptr) {
        std::fclose(file);
    }
}

VKS_STL_END
VKS_END