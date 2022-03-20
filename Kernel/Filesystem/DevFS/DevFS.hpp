#pragma once

#include "DevFSNode.hpp"

#include <Filesystem/Base/FS.hpp>
#include <Filesystem/Base/VNode.hpp>

namespace Kernel::FS {

class DevFS : public FS {
public:
    DevFS(VNodeStorage& vnode_storage);
    bool init();

    VNode& root() override { return m_root; };
    VNode* finddir(VNode& parent, const String& devname) override;
    uint32_t read(VNode& file, uint32_t offset, uint32_t size, void* buffer) override;
    uint32_t write(VNode& file, uint32_t offset, uint32_t size, void* buffer) override;
    bool mmap(VNode& file, uint32_t addr, uint32_t size) override;
    bool ioctl(VNode& file, uint32_t request) override;
    bool can_read(VNode& vnode, uint32_t offset) override;
    VNode* mkdir(VNode& directory, const String& name) override;
    void open(VNode& file, FileDescriptor& fd) override;

    VNode* create_device_node_inside_directory(VNode& directory, Device* device, const String& name);
    VNode* create_anonim_device_node(Device* device);

private:
    static DevFSNode& ToDevFSNode(VNode& device) { return reinterpret_cast<DevFSNode&>(device); }
    static DevFSNode* ToDevFSNode(VNode* device) { return reinterpret_cast<DevFSNode*>(device); }

private:
    DevFSNode m_root;
    VNodeStorage& m_vnode_storage;
    uint32_t devnodes {};
};

}