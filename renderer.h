#ifndef RENDERER_H
#define RENDERER_H

// ----------------------------------------------------------------------------------------------------

#include <vector>
#include <mutex>

#include "sokol_gfx.h"

#include "semaphore.h"

// ----------------------------------------------------------------------------------------------------

struct RENDER_COMMAND
{
	// types
	struct TYPE
	{
		enum ENUM
		{
			NOT_SET = 0,
			
			PUSH_DEBUG_GROUP,
			POP_DEBUG_GROUP,
			
			MAKE_BUFFER,
			MAKE_IMAGE,
			MAKE_SHADER,
			MAKE_PIPELINE,
			MAKE_PASS,
			DESTROY_BUFFER,
			DESTROY_IMAGE,
			DESTROY_SHADER,
			DESTROY_PIPELINE,
			DESTROY_PASS,
			UPDATE_BUFFER,
			APPEND_BUFFER,
			UPDATE_IMAGE,

			BEGIN_DEFAULT_PASS,
			BEGIN_PASS,
			APPLY_VIEWPORT,
			APPLY_SCISSOR_RECT,
			APPLY_PIPELINE,
			APPLY_BINDINGS,
			APPLY_UNIFORMS,
			DRAW,
			END_PASS,
			COMMIT,
			
			CUSTOM
		};
	};
	
	RENDER_COMMAND() {}
	RENDER_COMMAND(TYPE::ENUM _type) : type(_type) {}

	TYPE::ENUM type = TYPE::NOT_SET;

	union
	{
		struct
		{
			const char* name;
		} push_debug_group;

		struct
		{
			sg_buffer_desc desc;
			sg_buffer buffer;
		} make_buffer;
		
		struct
		{
			sg_image_desc desc;
			sg_image image;
		} make_image;
		
		struct
		{
			sg_shader_desc desc;
			sg_shader shader;
		} make_shader;
		
		struct
		{
			sg_pipeline_desc desc;
			sg_pipeline pipeline;
		} make_pipeline;
		
		struct
		{
			sg_pass_desc desc;
			sg_pass pass;
		} make_pass;
		
		struct
		{
			sg_buffer buffer;
		} destroy_buffer;
		
		struct
		{
			sg_image image;
		} destroy_image;
		
		struct
		{
			sg_shader shader;
		} destroy_shader;
		
		struct
		{
			sg_pipeline pipeline;
		} destroy_pipeline;
		
		struct
		{
			sg_pass pass;
		} destroy_pass;
		
		struct
		{
			sg_buffer buffer;
			sg_range data;
		} update_buffer;
		
		struct
		{
			sg_buffer buffer;
			sg_range data;
		} append_buffer;
		
		struct
		{
			sg_image image;
			sg_image_data data;
		} update_image;
		
		struct
		{
			void (*custom_cb)(void* custom_data);
			void* custom_data;
		} custom;

		struct
		{
			sg_pass_action pass_action;
		} begin_default_pass;

		struct
		{
			sg_pass pass;
			sg_pass_action pass_action;
		} begin_pass;

		struct
		{
			int x;
			int y;
			int width;
			int height;
			bool origin_top_left;
		} apply_viewport;
		
		struct
		{
			int x;
			int y;
			int width;
			int height;
			bool origin_top_left;
		} apply_scissor_rect;
		
		struct
		{
			sg_pipeline pipeline;
		} apply_pipeline;
		
		struct
		{
			sg_bindings bindings;
		} apply_bindings;
		
		struct
		{
			sg_shader_stage stage;
			int ub_index;
			size_t data_size;
			char buf[512];
		} apply_uniforms;
		
		struct
		{
			int base_element;
			int number_of_elements;
			int number_of_instances;
		} draw;
	};
};

// ----------------------------------------------------------------------------------------------------

typedef std::vector<RENDER_COMMAND> RENDER_COMMAND_ARRAY;

// ----------------------------------------------------------------------------------------------------

struct RENDER_CLEANUP
{
	RENDER_CLEANUP() {}
	RENDER_CLEANUP(void (*_cleanup_cb)(void* cleanup_data), void* _cleanup_data) : cleanup_cb(_cleanup_cb), cleanup_data(_cleanup_data) {}
	
	void (*cleanup_cb)(void* cleanup_data) = nullptr;
	void* cleanup_data = nullptr;
	int32_t frame_index = 0;
};

// ----------------------------------------------------------------------------------------------------

typedef std::vector<RENDER_CLEANUP> RENDER_CLEANUP_ARRAY;

// ----------------------------------------------------------------------------------------------------

class RENDERER
{
public:
	RENDERER(const sg_desc& desc);
	~RENDERER();

	// render thread functions
	void execute_commands(bool resource_only = false);
	void wait_for_flush();

	void set_default_pass_size(int width, int height) { m_default_pass_width = width; m_default_pass_height = height; }

	// update thread functions
	void add_command_push_debug_group(const char* name);
	void add_command_pop_debug_group();
	
	sg_buffer add_command_make_buffer(const sg_buffer_desc& desc);
	sg_image add_command_make_image(const sg_image_desc& desc);
	sg_shader add_command_make_shader(const sg_shader_desc& desc);
	sg_pipeline add_command_make_pipeline(const sg_pipeline_desc& desc);
	sg_pass add_command_make_pass(const sg_pass_desc& desc);
	
	void add_command_destroy_buffer(sg_buffer buffer);
	void add_command_destroy_image(sg_image image);
	void add_command_destroy_shader(sg_shader shader);
	void add_command_destroy_pipeline(sg_pipeline pipeline);
	void add_command_destroy_pass(sg_pass pass);
	
	void add_command_update_buffer(sg_buffer buffer, const sg_range& data);
	void add_command_append_buffer(sg_buffer buffer, const sg_range& data);
	void add_command_update_image(sg_image image, const sg_image_data& data);
	
	void add_command_begin_default_pass(const sg_pass_action& pass_action);
	void add_command_begin_pass(sg_pass pass, const sg_pass_action& pass_action);
	void add_command_apply_viewport(int x, int y, int width, int height, bool origin_top_left);
	void add_command_apply_scissor_rect(int x, int y, int width, int height, bool origin_top_left);
	void add_command_apply_pipeline(sg_pipeline pipeline);
	void add_command_apply_bindings(const sg_bindings& bindings);
	void add_command_apply_uniforms(sg_shader_stage stage, int ub_index, const sg_range& data);
	void add_command_draw(int base_element, int number_of_elements, int number_of_instances);
	void add_command_end_pass();
	void add_command_commit();

	void add_command_custom(void (*custom_cb)(void* custom_data), void* custom_data);
	
	void schedule_cleanup(void (*cleanup_cb)(void* cleanup_data), void* cleanup_data, int32_t number_of_frames_to_defer = 0);

	void commit_commands();
	void flush_commands();
	
	void lock_execute_mutex() { m_execute_mutex.lock(); }
	void unlock_execute_mutex() { m_execute_mutex.unlock(); }

	const std::string get_name() const;

	sg_pixel_format get_pixel_format() const { return sg_query_desc().context.color_format; }
	
private:
	void process_cleanups(int32_t frame_index);

	static void dealloc_buffer_cb(void* cleanup_data) { sg_dealloc_buffer({(uint32_t)(uintptr_t)cleanup_data}); }
	static void dealloc_image_cb(void* cleanup_data) { sg_dealloc_image({(uint32_t)(uintptr_t)cleanup_data}); }
	static void dealloc_shader_cb(void* cleanup_data) { sg_dealloc_shader({(uint32_t)(uintptr_t)cleanup_data}); }
	static void dealloc_pipeline_cb(void* cleanup_data) { sg_dealloc_pipeline({(uint32_t)(uintptr_t)cleanup_data}); }
	static void dealloc_pass_cb(void* cleanup_data) { sg_dealloc_pass({(uint32_t)(uintptr_t)cleanup_data}); }

	RENDER_COMMAND_ARRAY m_commands[2];
	int32_t m_pending_commands_index = 0;
	int32_t m_commit_commands_index = 1;
	RENDER_CLEANUP_ARRAY m_cleanups;

	SEMAPHORE m_update_semaphore;
	SEMAPHORE m_render_semaphore;
	std::atomic<bool> m_flushing = false;
	int m_default_pass_width = 0;
	int m_default_pass_height = 0;
	std::mutex m_execute_mutex;
	int32_t m_frame_index = 0;
};

// ----------------------------------------------------------------------------------------------------

typedef std::shared_ptr<RENDERER> RENDERER_REF;

#endif
