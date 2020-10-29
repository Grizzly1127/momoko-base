#ifndef MOMOKOBASE_NONCOPYABLE_H
#define MOMOKOBASE_NONCOPYABLE_H

namespace momoko
{

class noncopyable
{
public:
    noncopyable(const noncopyable&) = delete;
    void operator=(const noncopyable&) = delete;
protected:
    noncopyable() = default;
    ~noncopyable() = default;
};

} // namespace momoko

#endif // MOMOKOBASE_NONCOPYABLE_H
