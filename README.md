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

Also, includes a modified version of sokol_imgui.h as an example.