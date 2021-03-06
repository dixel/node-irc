srcdir = ''
blddir = 'lib'
VERSION = '0.0.1'

def set_options(opt):
  opt.tool_options('compiler_cxx')

def configure(conf):
  conf.check_tool('compiler_cxx')
  conf.check_tool('node_addon')
  conf.env.append_value("LIBPATH_IRC", "/usr/lib")
  conf.env.append_value("LIB_IRC", "ircclient")
  conf.env.append_unique('CXXFLAGS', ["-lpthread"])

def build(bld):
  obj = bld.new_task_gen('cxx', 'shlib', 'node_addon')
  obj.target = 'node-irc'
  obj.source = './binding.cc'
  obj.uselib = "IRC"
