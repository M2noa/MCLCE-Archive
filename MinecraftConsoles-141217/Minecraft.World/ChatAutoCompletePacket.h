#pragma once

#include "Packet.h"

class ChatAutoCompletePacket : public Packet
{
#if 0
	private String message;

	public ChatAutoCompletePacket() {

	}
	public ChatAutoCompletePacket(String message) {
		this.message = message;
	}

	@Override
		public void read(DataInputStream dis) throws IOException {
			message = readUtf(dis, ChatPacket.MAX_LENGTH);
	}

	@Override
		public void write(DataOutputStream dos) throws IOException {
			writeUtf(message, dos);
	}

	@Override
		public void handle(PacketListener listener) {
			listener.handleChatAutoComplete(this);
	}

	@Override
		public int getEstimatedSize() {
			return 2 + message.length() * 2;
	}

	public String getMessage() {
		return message;
	}

	@Override
		public String getDebugInfo() {
			return String.format("message='%s'", message);
	}

	@Override
		public boolean canBeInvalidated() {
			return true;
	}

	@Override
		public boolean isInvalidatedBy(Packet packet) {
			return true;
	}
#endif
};