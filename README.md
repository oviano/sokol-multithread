A simple wrapper for sokol_gfx (see https://github.com/floooh/sokol) enabling multithreaded rendering.

Typically you would use it as follows:

Render thread:

- create RENDERER instance, passing sg_desc to initialise sokol graphics
- call renderer->set_default_pass_size() and again whenever back buffer size changes
- call renderer->execute_commands() in render loop
- delete renderer instance on termination

Update thread:

- call renderer->add_command_xxx() commands in a similar manner to how you would call sg_xxx() commands
- call renderer->commit_commands() when you're done
- call renderer->flush_commands() on termination, and exit thread

There is also a modified version of sokol_imgui.h as an example.

Resources

When creating or updating resources, buffers etc. it is up to the caller to ensure that any pointers passed into an add_command_xxx() command remain valid until the actual underlying sg_xxx() command is issued from the render thread.

To help with this, you can supply an optional completion callback to certain add_command_xxx() calls which will be called after the command has been executed and access to the memory is no longer required.

To keep things simple, this call is always made in the same thread as the original renderer->add_command_xxx() was made.
