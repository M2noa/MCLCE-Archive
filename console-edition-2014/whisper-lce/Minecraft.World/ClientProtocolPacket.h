#pragma once

#include "Packet.h"

class ClientProtocolPacket : public Packet
{
#if 0
	private int protocolVersion;
	private String userName;

	// [EB]: Two fields below exist because we used to have a feature where we sent this
	//       information so people with dynamic proxies know where to connect us to.
	private String hostName;
	private int port;

	public ClientProtocolPacket() {
		// Needed
	}

	public ClientProtocolPacket(final int protocolVersion, final String userName, final String hostName, final int port) {
		this.protocolVersion = protocolVersion;
		this.userName = userName;
		this.hostName = hostName;
		this.port = port;
	}

	@Override
		public void read(DataInputStream dis) throws IOException {
			protocolVersion = dis.readByte();
			userName = readUtf(dis, Player.MAX_NAME_LENGTH);
			hostName = readUtf(dis, 255);
			port = dis.readInt();
	}

	@Override
		public void write(DataOutputStream dos) throws IOException {
			dos.writeByte(protocolVersion);
			writeUtf(userName, dos);
			writeUtf(hostName, dos);
			dos.writeInt(port);
	}

	@Override
		public void handle(PacketListener listener) {
			listener.handleClientProtocolPacket(this);
	}

	@Override
		public int getEstimatedSize() {
			return 1 + 2 + 2 * userName.length();
	}

	public int getProtocolVersion() {
		return protocolVersion;
	}

	public String getUserName() {
		return userName;
	}

	public String getHostName() {
		return hostName;
	}

	public int getPort() {
		return port;
	}

	@Override
		public String getDebugInfo() {
			return String.format("ver=%d, name='%s'", protocolVersion, userName);
	}
#endif
};