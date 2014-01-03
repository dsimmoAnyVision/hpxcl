// Copyright (c)    2013 Martin Stumpf
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#ifndef HPX_OPENCL_BUFFER_HPP__
#define HPX_OPENCL_BUFFER_HPP__

#include "server/buffer.hpp"

#include <hpx/include/components.hpp>
#include <hpx/lcos/future.hpp>

#include <vector>

#include "fwd_declarations.hpp"

namespace hpx {
namespace opencl { 


    //////////////////////////////////////
    /// @brief Device memory.
    ///
    /// Every buffer belongs to one \ref device.
    ///
    class buffer
      : public hpx::components::client_base<
          buffer, hpx::components::stub_base<server::buffer>
        >
    {
    
        typedef hpx::components::client_base<
            buffer, hpx::components::stub_base<server::buffer>
            > base_type;

        public:
            // Empty constructor, necessary for hpx purposes
            buffer(){}

            // Constructor
            buffer(hpx::future<hpx::naming::id_type> const& gid)
              : base_type(gid)
            {}
            
            // ///////////////////////////////////////////////
            // Exposed Component functionality
            // 
            
            /**
             *  @brief Get the size of the buffer
             *  @return The size of the buffer
             */
            hpx::lcos::future<size_t>
            size() const;

            // Read buffer
            /**
             *  @brief Reads data from the buffer
             *
             *  @param offset   The start position of the area to read.
             *  @param size     The size of the area to read.
             *  @return         An \ref event that triggers upon completion.<BR>
             *                  The actual data will be accessable via the
             *                  \ref event class.
             *  @see event
             */
            hpx::lcos::future<hpx::opencl::event>
            enqueue_read(size_t offset, size_t size) const;
            
            /**
             *  @brief Reads data from the buffer
             *
             *  This overloaded version accepts an event to wait for.
             *
             *  @param offset   The start position of the area to read.
             *  @param size     The size of the area to read.
             *  @param event    An \ref event that this read depends on. <BR>
             *                  The read will be executed after this event is
             *                  completed.
             *  @return         An \ref event that triggers upon completion.<BR>
             *                  The actual data will be accessable via the
             *                  \ref event class.
             *  @see event
             */
            hpx::lcos::future<hpx::opencl::event>
            enqueue_read(size_t offset, size_t size,
                                       hpx::opencl::event event) const;

            /**
             *  @brief Reads data from the buffer
             *
             *  This overloaded version accepts multiple events to wait for.
             *
             *  @param offset   The start position of the area to read.
             *  @param size     The size of the area to read.
             *  @param events   A list of \ref event "events" that this read
             *                  depends on.
             *                  <BR>
             *                  The read will be executed after all given events
             *                  are completed.
             *  @return         An \ref event that triggers upon completion.<BR>
             *                  The actual data will be accessable via the
             *                  \ref event class.
             *  @see event
             */
            hpx::lcos::future<hpx::opencl::event>
            enqueue_read(size_t offset, size_t size,
                                  std::vector<hpx::opencl::event> events) const;

            // Write Buffer
            /**
             *  @brief Writes data to the buffer
             *
             *  @param offset   The start position of the area to write to.
             *  @param size     The size of the data to write.
             *  @param data     The data to be written.
             *  @return         An \ref event that triggers upon completion.
             *                  
             *  @see event
             */
            hpx::lcos::future<hpx::opencl::event>
            enqueue_write(size_t offset, size_t size, const void* data) const;
            
            /**
             *  @brief Writes data to the buffer
             *
             *  This overloaded version accepts an event to wait for.
             *
             *  @param offset   The start position of the area to write to.
             *  @param size     The size of the data to write.
             *  @param data     The data to be written.
             *  @param event    An \ref event that this write depends on. <BR>
             *                  The write will be executed after this event is
             *                  completed.
             *  @return         An \ref event that triggers upon completion.
             *                  
             *  @see event
             */
            hpx::lcos::future<hpx::opencl::event>
            enqueue_write(size_t offset, size_t size, const void* data,
                                       hpx::opencl::event event) const;
            
            /**
             *  @brief Writes data to the buffer
             *
             *  This overloaded version accepts multiple events to wait for.
             *
             *  @param offset   The start position of the area to write to.
             *  @param size     The size of the data to write.
             *  @param data     The data to be written.
             *  @param events   A list of \ref event "events" that this write
             *                  depends on.
             *                  <BR>
             *                  The write will be executed after all given
             *                  events are completed.
             *  @return         An \ref event that triggers upon completion.
             *                  
             *  @see event
             */
            hpx::lcos::future<hpx::opencl::event>
            enqueue_write(size_t offset, size_t size, const void* data,
                                  std::vector<hpx::opencl::event> events) const;

            // Fill Buffer
            /**
             *  @brief Fills the buffer with a pattern
             *
             *  This function has the same argument restrictions as
             *  <A HREF="http://www.khronos.org/registry/cl/sdk/1.2/docs/man/xht
             *  ml/clEnqueueFillBuffer.html">
             *  clEnqueueFillBuffer</A>.
             *
             *  @param pattern  The pattern data.
             *  @param pattern_size
             *                  The size of the pattern.
             *  @param offset   The start position of the area to write to.
             *  @param size     The size of the area to write to.
             *  @return         An \ref event that triggers upon completion.
             *                  
             *  @see event
             */
            hpx::lcos::future<hpx::opencl::event>
            enqueue_fill(const void* pattern, size_t pattern_size,
                                       size_t offset, size_t size) const;
            
            /**
             *  @brief Fills the buffer with a pattern
             *
             *  This function has the same argument restrictions as
             *  <A HREF="http://www.khronos.org/registry/cl/sdk/1.2/docs/man/xht
             *  ml/clEnqueueFillBuffer.html">
             *  clEnqueueFillBuffer</A>.
             *
             *  This overloaded version accepts an event to wait for.
             *  
             *  @param pattern  The pattern data.
             *  @param pattern_size
             *                  The size of the pattern.
             *  @param offset   The start position of the area to write to.
             *  @param size     The size of the area to write to.
             *  @param event    An \ref event that this write depends on. <BR>
             *                  The write will be executed after this event is
             *                  completed.
             *  @return         An \ref event that triggers upon completion.
             *                  
             *  @see event
             */
            hpx::lcos::future<hpx::opencl::event>
            enqueue_fill(const void* pattern, size_t pattern_size,
                                       size_t offset, size_t size,
                                       hpx::opencl::event event) const;
            
            /**
             *  @brief Fills the buffer with a pattern
             *
             *  This function has the same argument restrictions as
             *  <A HREF="http://www.khronos.org/registry/cl/sdk/1.2/docs/man/xht
             *  ml/clEnqueueFillBuffer.html">
             *  clEnqueueFillBuffer</A>.
             *
             *  This overloaded version accepts multiple events to wait for.
             *  
             *  @param pattern  The pattern data.
             *  @param pattern_size
             *                  The size of the pattern.
             *  @param offset   The start position of the area to write to.
             *  @param size     The size of the area to write to.
             *  @param events   A list of \ref event "events" that this write
             *                  depends on.
             *                  <BR>
             *                  The write will be executed after all given
             *                  events are completed.
             *  @return         An \ref event that triggers upon completion.
             *                  
             *  @see event
             */
            hpx::lcos::future<hpx::opencl::event>
            enqueue_fill(const void* pattern, size_t pattern_size,
                                  size_t offset, size_t size,
                                  std::vector<hpx::opencl::event> events) const;
            
            // Copy Buffer
            /**
             *  @brief Copies data from another buffer.
             *
             *  The buffers do NOT need to be from the same device,
             *  neither do they have to be on the same node.
             *
             *  @param buffer       The source buffer.
             *  @param src_offset   The offset on the source buffer.
             *  @param dst_offset   The offset on the destination buffer.
             *  @param size         The size of the area to copy.
             *  @return         An \ref event on the destination \ref device
             *                  that triggers upon completion.
             *                  
             *  @see event
             */
            hpx::lcos::future<hpx::opencl::event>
            enqueue_copy(buffer src, size_t src_offset, size_t dst_offset,
                                 size_t size) const;

            /**
             *  @brief Copies data from another buffer.
             *
             *  The buffers do NOT need to be from the same device,
             *  neither do they have to be on the same node.
             *
             *  This overloaded version accepts an event to wait for.
             *  
             *  @param buffer       The source buffer.
             *  @param src_offset   The offset on the source buffer.
             *  @param dst_offset   The offset on the destination buffer.
             *  @param size         The size of the area to copy.
             *  @param event    An \ref event that this copy depends on. <BR>
             *                  The copy will be executed after this event is
             *                  completed.
             *                  <BR><B>
             *                  The event has to be an event from the source
             *                  \ref device!
             *                  </B>
             *  @return         An \ref event on the destination \ref device
             *                  that triggers upon completion.
             *                  
             *  @see event
             */
            hpx::lcos::future<hpx::opencl::event>
            enqueue_copy(buffer src, size_t src_offset, size_t dst_offset,
                                 size_t size,
                                 hpx::opencl::event event) const;

            /**
             *  @brief Copies data from another buffer.
             *
             *  The buffers do NOT need to be from the same device,
             *  neither do they have to be on the same node.
             *
             *  This overloaded version accepts multiple events to wait for.
             *  
             *  @param buffer       The source buffer.
             *  @param src_offset   The offset on the source buffer.
             *  @param dst_offset   The offset on the destination buffer.
             *  @param size         The size of the area to copy.
             *  @param events   A list of \ref event "events" that this copy
             *                  depends on.
             *                  <BR>
             *                  The copy will be executed after all given
             *                  events are completed.
             *                  <BR><B>
             *                  These events have to be events from the source
             *                  \ref device!
             *                  </B>
             *  @return         An \ref event on the destination \ref device
             *                  that triggers upon completion.
             *                  
             *  @see event
             */
            hpx::lcos::future<hpx::opencl::event>
            enqueue_copy(buffer src, size_t src_offset, size_t dst_offset,
                                 size_t size,
                                 std::vector<hpx::opencl::event> events) const;


            /* TODO
             * clEnqueueReadBufferRect
             * clEnqueueWriteBufferRect
             * clEnqueueCopyBuffer
             * clEnqueueCopyBufferRect
             */

    };

}}



#endif// HPX_OPENCL_BUFFER_HPP__
