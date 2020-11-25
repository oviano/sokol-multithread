A simple wrapper for sokol_gfx (see https://github.com/floooh/sokol) enabling multithreaded rendering.

Typically you would use it as follows:

Render thread

- create renderer instance, passing sg_desc to initialise sokol graphics
- call renderer->set_default_pass_size() and again whenever back buffer size changes
- call renderer->execute_commands() in render loop
- call renderer->wait_for_flush() on termination
- delete renderer instance

Update thread

- call renderer->add_command_xxx() commands in a similar manner to how you would call sg_xxx() commands
- call renderer->commit_commands() when you're done for the frame
- call renderer->flush_commands() on termination, before exiting the thread

There is also a modified version of sokol_imgui.h as an example.

Resources

When creating or updating resources (buffers, images etc) it is up to the caller to ensure that any pointers passed into an add_command_xxx() command remain valid until the underlying sg_xxx() command has been issued from the render thread.

To help with this, you can optionally schedule clean-ups via the renderer->schedule_cleanup() function which allows user-provided callbacks to be called after all the commands to create the resources have been executed. These can be used to free up the memory or clean-up any objects that own the memory and avoids the need for the wrapper to make any copies of data.
