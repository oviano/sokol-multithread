#ifndef RENDERER_H
#define RENDERER_H

// ----------------------------------------------------------------------------------------------------

#include <vector>
#include <mutex>
#include <condition_variable>

#include "sokol_gfx.h"

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
	
	TYPE::ENUM type = TYPE::NOT_SET;

	union
	{
		struct
		{
			const char* name;
		} push_debug_group;

		struct
		{
		} pop_debug_group;
		
		struct
		{
			sg_buffer_desc desc;
			sg_buffer buffer;
			void (*completion_cb)(void* completion_data);
			void* completion_data;
		} make_buffer;
		
		struct
		{
			sg_image_desc desc;
			sg_image image;
			void (*completion_cb)(void* completion_data);
			void* completion_data;
		} make_image;
		
		struct
		{
			sg_shader_desc desc;
			sg_shader shader;
			void (*completion_cb)(void* completion_data);
			void* completion_data;
		} make_shader;
		
		struct
		{
			sg_pipeline_desc desc;
			sg_pipeline pipeline;
			void (*completion_cb)(void* completion_data);
			void* completion_data;
		} make_pipeline;
		
		struct
		{
			sg_pass_desc desc;
			sg_pass pass;
			void (*completion_cb)(void* completion_data);
			void* completion_data;
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
			const void* data;
			int data_size;
			void (*completion_cb)(void* completion_data);
			void* completion_data;
		} update_buffer;
		
		struct
		{
			sg_buffer buffer;
			const void* data;
			int data_size;
			void (*completion_cb)(void* completion_data);
			void* completion_data;
		} append_buffer;
		
		struct
		{
			sg_image image;
			sg_image_content cont;
			void (*completion_cb)(void* completion_data);
			void* completion_data;
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
			unsigned char data[512];
			int data_size;
		} apply_uniforms;
		
		struct
		{
			int base_element;
			int number_of_elements;
			int number_of_instances;
		} draw;
		
		struct
		{
		} end_pass;
		
		struct
		{
		} commit;
	};
};

// ----------------------------------------------------------------------------------------------------

typedef std::vector<RENDER_COMMAND> RENDER_COMMAND_ARRAY;

// ----------------------------------------------------------------------------------------------------

class RENDERER
{
public:
	RENDERER(const sg_desc& desc);
	~RENDERER();

	// render thread functions
	void execute_commands();
	void wait_for_flush();

	void set_default_pass_size(int width, int height) { m_default_pass_width = width; m_default_pass_height = height; }

	// update thread functions
	void add_command_push_debug_group(const char* name);
	void add_command_pop_debug_group();
	
	sg_buffer add_command_make_buffer(const sg_buffer_desc& desc, void (*completion_cb)(void* completion_data) = nullptr, void* completion_data = nullptr);
	sg_image add_command_make_image(const sg_image_desc& desc, void (*completion_cb)(void* completion_data) = nullptr, void* completion_data = nullptr);
	sg_shader add_command_make_shader(const sg_shader_desc& desc, void (*completion_cb)(void* completion_data) = nullptr, void* completion_data = nullptr);
	sg_pipeline add_command_make_pipeline(const sg_pipeline_desc& desc, void (*completion_cb)(void* completion_data) = nullptr, void* completion_data = nullptr);
	sg_pass add_command_make_pass(const sg_pass_desc& desc, void (*completion_cb)(void* completion_data) = nullptr, void* completion_data = nullptr);
	
	void add_command_destroy_buffer(sg_buffer buffer);
	void add_command_destroy_image(sg_image image);
	void add_command_destroy_shader(sg_shader shader);
	void add_command_destroy_pipeline(sg_pipeline pipeline);
	void add_command_destroy_pass(sg_pass pass);
	
	void add_command_update_buffer(sg_buffer buffer, const void* data, int data_size, void (*completion_cb)(void* completion_data) = nullptr, void* completion_data = nullptr);
	void add_command_append_buffer(sg_buffer buffer, const void* data, int data_size, void (*completion_cb)(void* completion_data) = nullptr, void* completion_data = nullptr);
	void add_command_update_image(sg_image image, const sg_image_content& cont, void (*completion_cb)(void* completion_data) = nullptr, void* completion_data = nullptr);
	
	void add_command_begin_default_pass(const sg_pass_action& pass_action);
	void add_command_begin_pass(sg_pass pass, const sg_pass_action& pass_action);
	void add_command_apply_viewport(int x, int y, int width, int height, bool origin_top_left);
	void add_command_apply_scissor_rect(int x, int y, int width, int height, bool origin_top_left);
	void add_command_apply_pipeline(sg_pipeline pipeline);
	void add_command_apply_bindings(const sg_bindings& bindings);
	void add_command_apply_uniforms(sg_shader_stage stage, int ub_index, const void* data, int data_size);
	void add_command_draw(int base_element, int number_of_elements, int number_of_instances);
	void add_command_end_pass();
	void add_command_commit();

	void add_command_custom(void (*custom_cb)(void* custom_data), void* custom_data);
	
	void commit_commands();
	void flush_commands();
	
private:
	void clear_commands(int32_t commands_index);

	RENDER_COMMAND_ARRAY m_commands[2];
	int32_t m_pending_commands_index = 0;
	int32_t m_commit_commands_index = 1;

	class SEMAPHORE
	{
	public:
		SEMAPHORE(uint32_t max_count = 0xFFFFFFFF) : m_max_count(max_count) {}
		~SEMAPHORE() {}
		
		void release()
		{
			std::scoped_lock<std::mutex> lock(m_mutex);
			if (m_count < m_max_count)
			{
				m_count ++;
			}
			m_cv.notify_one();
		}
		
		void acquire()
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			while(!m_count)
			{
				m_cv.wait(lock);
			}
			m_count --;
		}
		
	private:
		uint32_t m_count = 0;
		uint32_t m_max_count = 1;
		std::mutex m_mutex;
		std::condition_variable m_cv;
	};

	SEMAPHORE m_update_semaphore;
	SEMAPHORE m_render_semaphore;
	std::atomic<bool> m_flushing = false;
	int m_default_pass_width = 0;
	int m_default_pass_height = 0;
};

// ----------------------------------------------------------------------------------------------------

#endif
