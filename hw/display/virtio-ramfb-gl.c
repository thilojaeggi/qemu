#include "qemu/osdep.h"
#include "hw/pci/pci.h"
#include "hw/qdev-properties.h"
#include "hw/virtio/virtio-gpu.h"
#include "hw/display/vga.h"
#include "qapi/error.h"
#include "qemu/module.h"
#include "virtio-ramfb.h"
#include "qom/object.h"

/* Forward declaration of the realize function from virtio-ramfb.c */
static void virtio_ramfb_realize(VirtIOPCIProxy *vpci_dev, Error **errp);

#define TYPE_VIRTIO_RAMFB_GL "virtio-ramfb-gl"

typedef struct VirtIORAMFBGL VirtIORAMFBGL;
DECLARE_INSTANCE_CHECKER(VirtIORAMFBGL, VIRTIO_RAMFB_GL,
                         TYPE_VIRTIO_RAMFB_GL)

struct VirtIORAMFBGL {
    VirtIORAMFBBase parent_obj;

    VirtIOGPUGL   vdev;
};

static void virtio_ramfb_gl_inst_initfn(Object *obj)
{
    VirtIORAMFBGL *dev = VIRTIO_RAMFB_GL(obj);
    
    virtio_instance_init_common(obj, &dev->vdev, sizeof(dev->vdev),
                                TYPE_VIRTIO_GPU_GL);
    VIRTIO_RAMFB_BASE(dev)->vgpu = VIRTIO_GPU_BASE(&dev->vdev);
}

static void virtio_ramfb_gl_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    VirtioPCIClass *k = VIRTIO_PCI_CLASS(klass);
    PCIDeviceClass *pcidev_k = PCI_DEVICE_CLASS(klass);

    set_bit(DEVICE_CATEGORY_DISPLAY, dc->categories);
    dc->hotpluggable = false;
    k->realize = virtio_ramfb_realize;
    pcidev_k->class_id = PCI_CLASS_DISPLAY_OTHER;
}

static VirtioPCIDeviceTypeInfo virtio_ramfb_gl_info = {
    .generic_name  = TYPE_VIRTIO_RAMFB_GL,
    .parent        = TYPE_VIRTIO_RAMFB_BASE,
    .instance_size = sizeof(VirtIORAMFBGL),
    .instance_init = virtio_ramfb_gl_inst_initfn,
    .class_init    = virtio_ramfb_gl_class_init,
};
module_obj(TYPE_VIRTIO_RAMFB_GL);

static void virtio_ramfb_register_types(void)
{
    if (have_vga) {
        virtio_pci_types_register(&virtio_ramfb_gl_info);
    }
}

type_init(virtio_ramfb_register_types)

module_dep("hw-display-virtio-ramfb"); 