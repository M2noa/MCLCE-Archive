#pragma once

#include "Packet.h"

class SharedKeyPacket : public Packet
{
#if 0
	private byte[] keybytes = new byte[]{};
	private byte[] nonce = new byte[]{};

	private SecretKey secretKey;

	public SharedKeyPacket() {
		// Needed
	}

	public SharedKeyPacket(final SecretKey secretKey, final PublicKey publicKey, final byte[] nonce) {
		this.secretKey = secretKey;
		this.keybytes = Crypt.encryptUsingKey(publicKey, secretKey.getEncoded());
		this.nonce = Crypt.encryptUsingKey(publicKey, nonce);
	}

	@Override
		public void read(DataInputStream dis) throws IOException {
			keybytes = readBytes(dis);
			nonce = readBytes(dis);
	}

	@Override
		public void write(DataOutputStream dos) throws IOException {
			writeBytes(dos, keybytes);
			writeBytes(dos, nonce);
	}

	@Override
		public void handle(PacketListener listener) {
			listener.handleSharedKey(this);
	}

	@Override
		public int getEstimatedSize() {
			return 2 + keybytes.length + 2 + nonce.length;
	}

	public SecretKey getSecretKey(PrivateKey privateKey) {
		if (privateKey == null) {
			return secretKey;
		}
		return secretKey = Crypt.decryptByteToSecretKey(privateKey, keybytes);
	}

	public SecretKey getSecretKey() {
		return getSecretKey(null);
	}

	public byte[] getNonce(PrivateKey privateKey) {
		if (privateKey == null) {
			return nonce;
		}
		return Crypt.decryptUsingKey(privateKey, nonce);
	}
#endif
};