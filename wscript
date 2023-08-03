## -*- Mode: python; py-indent-offset: 4; indent-tabs-mode: nil; coding: utf-8; -*-

def build(bld):
    obj = bld.create_ns3_module('maher', ['internet', 'wifi'])

    obj.source = [
        'model/maher-information-element-vector.cc',
        'model/maher-point-device.cc',
        'model/maher-l2-routing-protocol.cc',
        'model/maher-wifi-beacon.cc',
        'model/maher-wifi-interface-mac.cc',
        'model/ie-dot11s-beacon-timing.cc',
        'model/ie-dot11s-configuration.cc',
        'model/ie-dot11s-id.cc',
        'model/ie-dot11s-peer-management.cc',
        'model/ie-dot11s-preq.cc',
        'model/ie-dot11s-prep.cc',
        'model/ie-dot11s-perr.cc',
        'model/ie-dot11s-rann.cc',
        'model/ie-dot11s-peering-protocol.cc',
        'model/ie-dot11s-metric-report.cc',
        'model-mac-header.cc',
        'model/peer-link-frame.cc',
        'model/peer-link.cc',
        'model/peer-management-protocol-mac.cc',
        'model/peer-management-protocol.cc',
        'model/hwmp-tag.cc',
        'model/hwmp-rtable.cc',
        'model/hwmp-protocol-mac.cc',
        'model/hwmp-protocol.cc',
        'model/airtime-metric.cc',
        'model/flame/flame-header.cc',
        'model/flame/flame-rtable.cc',
        'model/flame/flame-protocol-mac.cc',
        'model/flame/flame-protocol.cc',
        'helper/maher-helper.cc',
        'helper/maher-stack-installer.cc',
        'helper-installer.cc',
        'helper/flame/flame-installer.cc',
        ]

    obj_test = bld.create_ns3_module_test_library('maher')
    obj_test.source = [
        'test/maher-information-element-vector-test-suite.cc',
        'test-test-suite.cc',
        'test/pmp-regression.cc',
        'test/hwmp-reactive-regression.cc',
        'test/hwmp-proactive-regression.cc',
        'test/hwmp-simplest-regression.cc',
        'test/hwmp-target-flags-regression.cc',
        'test/regression.cc',
        'test/flame/flame-test-suite.cc',
        'test/flame/flame-regression.cc',
        'test/flame/regression.cc',
        ]

    headers = bld(features='ns3header')
    headers.module = 'maher'
    headers.source = [
        'model/maher-information-element-vector.h',
        'model/maher-point-device.h',
        'model/maher-l2-routing-protocol.h',
        'model/maher-wifi-beacon.h',
        'model/maher-wifi-interface-mac.h',
        'model/maher-wifi-interface-mac-plugin.h',
        'model/hwmp-protocol.h',
        'model/peer-management-protocol.h',
        'model/ie-dot11s-beacon-timing.h',
        'model/ie-dot11s-configuration.h',
        'model/ie-dot11s-peer-management.h',
        'model/ie-dot11s-id.h',
        'model/peer-link.h',
        'model-mac-header.h',
        'model/peer-link-frame.h',
        'model/hwmp-rtable.h',
        'model/ie-dot11s-peering-protocol.h',
        'model/ie-dot11s-metric-report.h',
        'model/ie-dot11s-perr.h',
        'model/ie-dot11s-prep.h',
        'model/ie-dot11s-preq.h',
        'model/ie-dot11s-rann.h',
        'model/flame/flame-protocol.h',
        'model/flame/flame-header.h',
        'model/flame/flame-rtable.h',
        'model/flame/flame-protocol-mac.h',
        'helper/maher-helper.h',
        'helper/maher-stack-installer.h',
        'helper-installer.h',
        'helper/flame/flame-installer.h',
        ]

    if bld.env['ENABLE_EXAMPLES']:
        bld.recurse('examples')

    bld.ns3_python_bindings()
