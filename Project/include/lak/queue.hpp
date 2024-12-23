/*
MIT License

Copyright (c) 2018 LAK132

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef LAK_QUEUE_H
#define LAK_QUEUE_H

#include <memory>
#include <mutex>

namespace lak
{
    struct queue_t;
    struct _ticket;
    using ticket_t = std::shared_ptr<_ticket>;

    struct _ticket
    {
    private:
        shared_ptr<std::mutex> mtx = make_shared<std::mutex>();
        std::unique_ptr<std::lock_guard<std::mutex>> guard;
    public:
        _ticket()
        {
            guard = std::make_unique<std::lock_guard<std::mutex>>(*mtx);
        }
        friend queue_t;
    };

    struct queue_t
    {
    private:
        weak_ptr<_ticket> tail;
        std::mutex _lock;
    public:
        ticket_t lock()
        {
            _lock.lock();
            shared_ptr<std::mutex> mtx = nullptr;
            {
                ticket_t oldt = tail.lock();
                if(oldt != nullptr) mtx = oldt->mtx;
            }
            ticket_t newt = make_shared<_ticket>();
            tail = newt;
            _lock.unlock();

            if(mtx != nullptr) std::lock_guard<std::mutex> wait(*mtx);
            return newt;
        }
    };
}

#endif // LAK_QUEUE_H