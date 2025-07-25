#include <vks/__stl/raii.hpp>

VKS_BEGIN
VKS_STL_BEGIN

void FileCloser::operator()(FILE *file)
{
    if (file != nullptr) {
        std::fclose(file);
    }
}

VKS_STL_END
VKS_END