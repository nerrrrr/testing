#include "rbuffer.h"

void RingBuffer::Reset()
{
	std::lock_guard<std::mutex> lock(mutex_);
	head_ = tail_;
	full_ = false;
}


// State Tracking
bool RingBuffer::Empty() const
{
	//if head and tail are equal, we are empty
	return (!full_ && (head_ == tail_));
}

bool RingBuffer::Full() const
{
	//If tail is ahead the head by 1, we are full
	return full_;
}

size_t RingBuffer::Capacity() const
{
	return max_size_;
}

size_t RingBuffer::Size() const
{
	size_t size = max_size_;

	if(!full_)
	{
		if(head_ >= tail_)
		{
			size = head_ - tail_;
		}
		else
		{
			size = max_size_ + head_ - tail_;
		}
	}

	return size;
}

// Adding Data
void RingBuffer::Put(const double item[])
{
	std::lock_guard<std::mutex> lock(mutex_);

   for(int i = 0; i < m_buffer_number; i++)
   {
	   buf_[i][head_] = item[i];
   }

	if(full_)
	{
		tail_ = (tail_ + 1) % max_size_;
	}

	head_ = (head_ + 1) % max_size_;

	full_ = head_ == tail_;
}

// Retrieving Data
void RingBuffer::Get(double returned_data[])
{
	std::lock_guard<std::mutex> lock(mutex_);

	if(Empty())
	{
		returned_data = NULL;
	}

	//Read data and advance the tail (we now have a free space)

   for(int i = 0; i < m_buffer_number; i++)
   {
      returned_data[i] =  buf_[i][tail_];
   }

	full_ = false;
	tail_ = (tail_ + 1) % max_size_;
}

void RingBuffer::Dump(int marker, double* history[], double* shot[])
{
	std::lock_guard<std::mutex> lock(mutex_);

	if(Empty())
	{
    history = NULL;
    shot = NULL;
	}

	//Read data

  for(int i = 0; i < marker; i++)
  {
    Look(i, history[i]);
  }
  for(int i = marker; i < max_size_; i++)
  {
    Look(i, shot[i-marker]);
  }
}


//  Looks at the index from the tail and puts data into your array
void RingBuffer::Look(int index, double returned_data[])
{
	std::lock_guard<std::mutex> lock(mutex_);

	if(Empty())
	{
    returned_data = NULL;
	}

	//Read data and save it to returned_data

   for(int i = 0; i < m_buffer_number; i++)
   {
      returned_data[i] =  buf_[i][(tail_+index) % max_size_];
   }
}
