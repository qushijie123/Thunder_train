#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xd9726f80, "module_layout" },
	{ 0xe340d421, "device_destroy" },
	{ 0x6bc3fbc0, "__unregister_chrdev" },
	{ 0xc6f46339, "init_timer_key" },
	{ 0x9f4f34bc, "device_create" },
	{ 0x64b60eb0, "class_destroy" },
	{ 0xa946dcde, "__class_create" },
	{ 0x4102c7cb, "__register_chrdev" },
	{ 0x37874eb8, "wake_up_process" },
	{ 0x40794748, "kthread_create_on_node" },
	{ 0x6729d3df, "__get_user_4" },
	{ 0xc38c83b8, "mod_timer" },
	{ 0x15ba50a6, "jiffies" },
	{ 0x2b68bd2f, "del_timer" },
	{ 0xe9ffc063, "down_trylock" },
	{ 0x24d273d1, "add_timer" },
	{ 0xcf2a6966, "up" },
	{ 0xc5850110, "printk" },
	{ 0xbdfb6dbb, "__fentry__" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "7665A8FF1F490363DBE4F93");
