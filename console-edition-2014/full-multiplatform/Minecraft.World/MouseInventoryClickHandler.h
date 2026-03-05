#pragma once

// 4J The body of this class was commented out in Java. Copying here for completeness

class MouseInventoryClickHandler
{
/*	public static void handle(int buttonNum, boolean clickedOutside, int slotId, Player player) {
		Inventory inventory = player.inventory;
		Slot slot = player.getInventorySlot(slotId);
		if (slot != null) {
			ItemInstance clicked = slot.getItem();
			if (clicked == null && inventory.carried == null) {
			} else if (clicked != null && inventory.carried == null) {
				int c = buttonNum == 0 ? clicked.count : (clicked.count + 1) / 2;
				inventory.carried = slot.container.removeItem(slot.slot, c);
				if (clicked.count == 0) slot.set(null);
				slot.onTake();
			} else if (clicked == null && inventory.carried != null && slot.mayPlace(inventory.carried)) {
				int c = buttonNum == 0 ? inventory.carried.count : 1;
				if (c > slot.getMaxStackSize()) c = slot.getMaxStackSize();
				slot.set(inventory.carried.remove(c));
				if (inventory.carried.count == 0) inventory.carried = null;
			} else if (clicked != null && inventory.carried != null) {

				if (slot.mayPlace(inventory.carried)) {
					if (clicked.id != inventory.carried.id) {
						if (inventory.carried.count <= slot.getMaxStackSize()) {
							ItemInstance tmp = clicked;
							slot.set(inventory.carried);
							inventory.carried = tmp;
						}
					} else if (clicked.id == inventory.carried.id) {
						if (buttonNum == 0) {
							int c = inventory.carried.count;
							if (c > slot.getMaxStackSize() - clicked.count) c = slot.getMaxStackSize() - clicked.count;
							if (c > inventory.carried.getMaxStackSize() - clicked.count) c = inventory.carried.getMaxStackSize() - clicked.count;
							inventory.carried.remove(c);
							if (inventory.carried.count == 0) inventory.carried = null;
							clicked.count += c;
						} else if (buttonNum == 1) {
							int c = 1;
							if (c > slot.getMaxStackSize() - clicked.count) c = slot.getMaxStackSize() - clicked.count;
							if (c > inventory.carried.getMaxStackSize() - clicked.count) c = inventory.carried.getMaxStackSize() - clicked.count;
							inventory.carried.remove(c);
							if (inventory.carried.count == 0) inventory.carried = null;
							clicked.count += c;
						}
					}
				} else {
					if (clicked.id == inventory.carried.id && inventory.carried.getMaxStackSize() > 1) {
						int c = clicked.count;
						if (c > 0 && c + inventory.carried.count <= inventory.carried.getMaxStackSize()) {
							inventory.carried.count += c;
							clicked.remove(c);
							if (clicked.count == 0) slot.set(null);
							slot.onTake();
						}
					}
				}
			}
			slot.setChanged();
		} else if (inventory.carried != null) {
			if (clickedOutside) {
				if (buttonNum == 0) {
					player.drop(inventory.carried);
					inventory.carried = null;
				}
				if (buttonNum == 1) {
					player.drop(inventory.carried.remove(1));
					if (inventory.carried.count == 0) inventory.carried = null;
				}
			}
		}
	}

	public static void handleClose(Player player) {
		Inventory inventory = player.inventory;
		if (inventory.carried != null) {
			player.drop(inventory.carried);
			inventory.carried = null;
		}
	}*/

};