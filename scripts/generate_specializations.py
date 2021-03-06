rotation = [0x1B1, 0x18E, 0x076, 0x16A, 0x155, 0x0AD, 0x0DC, 0x0E3, 0x11B,
            0x0A1, 0x10A, 0x046, 0x062, 0x054, 0x111, 0x200]
translation = [0, 1]

output_string = """\
if (trans_code == {:d} && rot_code == {:#05x}) {{
  return Factory<VolumeType>::template Create<{:d}, {:#05x}>(
           logical_volume, matrix
         );
}}\
"""

for r in rotation:
  for t in translation:
    print(output_string.format(t, r, t, r))