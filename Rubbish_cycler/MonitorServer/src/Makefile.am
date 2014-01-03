AUTOMAKE_OPTIONS = foreign

noinst_LIBRARIES = libCS.a

libCS_a_SOURCES = AgentRegister.cpp \
				   CSConfiguration.cpp \
				   CSNoticeAgent.cpp \
				   CSTCPAgent.cpp \
				   CS_Epoll.cpp \
				   Check_heartbeat_timer.cpp \
				   Dispatcher.cpp \
				   Extent_task.cpp \
				   Get_rule_task.cpp \
				   HeartbeatAgent.cpp \
				   Init_rule.cpp \
				   Lib_tool.cpp \
				   Log_warp.cpp \
				   MPCClientAgent.cpp \
				   Migration_task.cpp \
				   RSClientAgent.cpp \
				   RulerManager.cpp \
				   RunControl.cpp \
				   SCAgent.cpp \
				   SystemAgent.cpp \
				   Task.cpp \
				   Task_manager.cpp \
				   ThreadPool.cpp \
				   TimerAgent.cpp \
				   WorkItem.cpp  \
				   Migration_work_item.cpp \
				   Extent_work_item.cpp \
				   Rule_file_interface.cpp \
				   HB_manager.cpp \
				   DB_operation.cpp \
				   Init_system_task.cpp \
				   Init_system_work_item.cpp \
				   Register_agent.cpp   \
				   rubbish_recycler.cpp \
				   RC_agent.cpp \
				   RC_check_heartbeat.h 


libCS_a_LIBADD = $(top_srcdir)/build/common/libcommon.a

INCLUDES = $(GLOBAL_INCLUDES)

LIBS += $(GLOBAL_LIBS)

CXXFLAGS += -g -pg -Wall -O0
