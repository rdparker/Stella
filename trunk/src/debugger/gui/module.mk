MODULE := src/debugger/gui

MODULE_OBJS := \
	src/debugger/gui/AudioWidget.o \
	src/debugger/gui/CpuWidget.o \
	src/debugger/gui/PromptWidget.o \
	src/debugger/gui/RamWidget.o \
	src/debugger/gui/RiotWidget.o \
	src/debugger/gui/RomWidget.o \
	src/debugger/gui/RomListWidget.o \
	src/debugger/gui/TiaWidget.o \
	src/debugger/gui/TiaInfoWidget.o \
	src/debugger/gui/TiaOutputWidget.o \
	src/debugger/gui/TiaZoomWidget.o \
	src/debugger/gui/ColorWidget.o \
	src/debugger/gui/DataGridOpsWidget.o \
	src/debugger/gui/DataGridWidget.o \
	src/debugger/gui/DebuggerDialog.o \
	src/debugger/gui/ToggleBitWidget.o \
	src/debugger/gui/TogglePixelWidget.o \
	src/debugger/gui/ToggleWidget.o \
	src/debugger/gui/Cart0840Widget.o \
	src/debugger/gui/Cart2KWidget.o \
	src/debugger/gui/Cart3FWidget.o \
	src/debugger/gui/Cart4KWidget.o \
	src/debugger/gui/CartCVWidget.o \
	src/debugger/gui/CartEFWidget.o \
	src/debugger/gui/CartEFSCWidget.o \
	src/debugger/gui/CartF0Widget.o \
	src/debugger/gui/CartFAWidget.o \
	src/debugger/gui/CartF4Widget.o \
	src/debugger/gui/CartF6Widget.o \
	src/debugger/gui/CartF8Widget.o \
	src/debugger/gui/CartF4SCWidget.o \
	src/debugger/gui/CartF6SCWidget.o \
	src/debugger/gui/CartF8SCWidget.o \
	src/debugger/gui/CartUAWidget.o \
	src/debugger/gui/JoystickWidget.o \
	src/debugger/gui/PaddleWidget.o \
	src/debugger/gui/BoosterWidget.o \
	src/debugger/gui/DrivingWidget.o \
	src/debugger/gui/KeyboardWidget.o \
	src/debugger/gui/GenesisWidget.o

MODULE_DIRS += \
	src/debugger/gui

# Include common rules 
include $(srcdir)/common.rules
