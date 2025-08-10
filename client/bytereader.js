export function readUInt16(bytes, offset) {
  const buffer = new ArrayBuffer(2);
  const view = new Uint8Array(buffer);

  view[0] = bytes[offset];
  view[1] = bytes[offset + 1];

  return new DataView(buffer).getUint16(0, true);
}
export function readInt32(bytes, offset) {
  const buffer = new ArrayBuffer(4);
  const view = new Uint8Array(buffer);

  view[0] = bytes[offset];
  view[1] = bytes[offset + 1];
  view[2] = bytes[offset + 2];
  view[3] = bytes[offset + 3];

  return new DataView(buffer).getInt32(0, true);
}

export function readUInt32(bytes, offset) {
  const buffer = new ArrayBuffer(4);
  const view = new Uint8Array(buffer);

  view[0] = bytes[offset];
  view[1] = bytes[offset + 1];
  view[2] = bytes[offset + 2];
  view[3] = bytes[offset + 3];

  return new DataView(buffer).getUint32(0, true);
}

export function readFloat32(bytes, offset) {
  const buffer = new ArrayBuffer(4);
  const view = new Uint8Array(buffer);

  view[0] = bytes[offset];
  view[1] = bytes[offset + 1];
  view[2] = bytes[offset + 2];
  view[3] = bytes[offset + 3];

  return new DataView(buffer).getFloat32(0, true);
}

export function readString(bytes, offset, length) {
  let chars = [];

  for (let i = 0; i < length; i++) {
    const byte = bytes[offset + i];
    chars.push(String.fromCharCode(byte));
  }

  return chars.join("");
}
