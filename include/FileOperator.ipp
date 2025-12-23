FileOperator::FileOperator(const std::string &file_name)noexcept:file_name(file_name)
{
    file.close();
    file.open(file_name,std::ios::in|std::ios::out|std::ios::binary);
    if(!file)
    {
#ifdef DEBUG
        std::cout<<"makefile "<<file_name<<std::endl;
#endif
        file.open(file_name,std::ios::out|std::ios::binary);
        file.close();
        file.open(file_name,std::ios::in|std::ios::out|std::ios::binary);
    }
}
template<typename T> void FileOperator::read(int pos,T *t) noexcept
{
    file.seekg(pos);
    file.read(reinterpret_cast<char*>(t),T::memory_size);
}
template<typename T> void FileOperator::write(int pos,T *t) noexcept
{
    file.seekp(pos);
    file.write(reinterpret_cast<char*>(t),T::memory_size);
}
void FileOperator::read(int pos,int *t) noexcept
{
    file.seekg(pos);
    file.read(reinterpret_cast<char*>(t),sizeof(int));
}
void FileOperator::write(int pos,int *t) noexcept
{
    file.seekp(pos);
    file.write(reinterpret_cast<char*>(t),sizeof(int));
}
int FileOperator::size() noexcept
{
    file.seekg(0,std::ios::end);
    return file.tellg();
}