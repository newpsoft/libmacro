"""Test script for libmacro Python bindings."""
import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'build', 'python'))
import _libmacro as m


# ── Stateless tests (no Libmacro context needed) ──

def test_enums():
    assert m.Dimension.X == 0
    assert m.Dimension.Y == 1
    assert m.Dimension.Z == 2
    assert m.Dimension.W == 3

    assert m.ApplyValue.SET == 0
    assert m.ApplyValue.UNSET == 1
    assert m.ApplyValue.BOTH == 2
    assert m.ApplyValue.TOGGLE == 3

    assert m.ModFlags.NONE == 0
    assert m.ModFlags.ALT == 1
    assert m.ModFlags.CTRL == 64

    assert m.TriggerMode.EQUAL == 0
    assert m.TriggerMode.ALL == 1
    assert m.TriggerMode.ANY == 6

    assert m.InterruptValue.CONTINUE == 0
    assert m.InterruptValue.PAUSE == 1
    assert m.InterruptValue.INTERRUPT == 2
    assert m.InterruptValue.INTERRUPT_ALL == 3
    assert m.InterruptValue.DISABLE == 4


def test_flags_helpers():
    combined = m.flags_combine([m.ModFlags.ALT, m.ModFlags.CTRL])
    assert combined == (1 | 64)
    added = m.flags_add(combined, m.ModFlags.SHIFT)
    assert m.flags_has(added, m.ModFlags.ALT)
    assert m.flags_has(added, m.ModFlags.CTRL)
    assert m.flags_has(added, m.ModFlags.SHIFT)
    assert not m.flags_has(added, m.ModFlags.META)
    removed = m.flags_remove(added, m.ModFlags.CTRL)
    assert not m.flags_has(removed, m.ModFlags.CTRL)


def test_space_position():
    sp = m.SpacePosition()
    sp.x = 10
    sp.y = 20
    sp.z = 30
    assert sp.x == 10
    assert sp.y == 20
    assert sp.z == 30
    assert sp.w == 0


def test_signals():
    mod = m.Modifier()
    assert mod.name() == 'Modifier'
    mod.modifiers = m.flags_combine([m.ModFlags.CTRL])
    mod.apply = m.ApplyValue.SET
    assert mod.apply == m.ApplyValue.SET

    noop = m.NoOp()
    assert noop.name() == 'NoOp'

    isig = m.Interrupt()
    assert isig.name() == 'Interrupt'

    fired = []
    def on_signal():
        fired.append(True)
    fsig = m.FunctorSignal(on_signal)
    fsig.send()
    assert len(fired) == 1


def test_triggers():
    act = m.Action()
    assert act.name() == 'Action'
    act.modifiers = m.flags_combine([m.ModFlags.CTRL])
    act.trigger_mode = m.TriggerMode.EQUAL
    assert act.trigger_mode == m.TriggerMode.EQUAL

    fired = []
    def on_trigger(sig, mods):
        fired.append(True)
        return True
    ftrig = m.FunctorTrigger(on_trigger)


# ── Context-dependent tests (share one Libmacro instance) ──

def test_libmacro_context(lib):
    assert lib is not None
    assert lib.enabled is True
    lib.enabled = False
    assert lib.enabled is False
    lib.enabled = True


def test_macro(lib):
    macro = m.create_macro(lib)
    assert macro is not None
    assert macro.context is not None

    assert macro.name is None or macro.name == ''
    macro.name = 'test_macro'
    assert macro.name == 'test_macro'

    assert macro.blocking is False
    macro.blocking = True
    assert macro.blocking is True

    assert macro.sticky is False
    macro.sticky = True
    assert macro.sticky is True

    assert macro.thread_max == 1
    macro.thread_max = 4
    assert macro.thread_max == 4

    assert macro.enabled is False
    macro.enabled = True
    assert macro.enabled is True


def test_registries(lib):
    serial = lib.serial()
    sig_reg = lib.signal_registry()
    trig_reg = lib.trigger_registry()
    macro_reg = lib.macro_registry()
    assert serial is not None
    assert sig_reg is not None
    assert trig_reg is not None
    assert macro_reg is not None

    name = serial.apply_value_name(m.ApplyValue.SET)
    assert name is not None
    assert name == 'Set'

    dim = serial.dimension_name(m.Dimension.X)
    assert dim is not None


def test_macro_signals_triggers(lib):
    macro = m.create_macro(lib)

    mod = m.Modifier()
    mod.modifiers = m.flags_combine([m.ModFlags.CTRL])
    mod.apply = m.ApplyValue.SET

    noop = m.NoOp()

    act = m.Action()
    act.modifiers = m.flags_combine([m.ModFlags.CTRL])
    act.trigger_mode = m.TriggerMode.EQUAL

    macro.set_signals([mod, noop])
    macro.set_triggers([act])

    macro.clear_signals()
    macro.clear_triggers()
    macro.clear_all()


# ── Main ──

if __name__ == '__main__':
    # Stateless tests first
    test_enums()
    print('  enums OK')
    test_flags_helpers()
    print('  flags helpers OK')
    test_space_position()
    print('  SpacePosition OK')
    test_signals()
    print('  signals OK')
    test_triggers()
    print('  triggers OK')

    # Shared context tests
    ctx = m.create_context()
    lib = m.Libmacro.instance()

    test_libmacro_context(lib)
    print('  Libmacro context OK')
    test_macro(lib)
    print('  macro OK')
    test_registries(lib)
    print('  registries OK')
    test_macro_signals_triggers(lib)
    print('  macro signals/triggers OK')

    # Clean up
    lib.enabled = False
    print('All tests passed!')
