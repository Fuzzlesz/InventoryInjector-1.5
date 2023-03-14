#include "MovieManager.h"

#include "Hooks/FavoritesListEntry.h"
#include "Hooks/GroupDataExtender.h"
#include "Hooks/IconSetter.h"
#include "Hooks/InventoryListEntry.h"

namespace Hooks
{
	MovieManager* MovieManager::GetSingleton()
	{
		static MovieManager singleton{};
		return &singleton;
	}

	void MovieManager::Install()
	{
		auto hook = REL::Relocation<std::uintptr_t>(
			REL::ID(80302), 0x1D9);

		if (!REL::make_pattern<"FF 15">().match(hook.address())) {
			util::report_and_fail("Failed to install MovieManager");
		}

		auto& trampoline = SKSE::GetTrampoline();
		auto ptr = trampoline.write_call<6>(hook.address(), &AddScaleformHooks);
		_SetViewScaleMode = *reinterpret_cast<std::uintptr_t*>(ptr);
	}

	void MovieManager::AddScaleformHooks(
		RE::GFxMovieView* a_view,
		RE::GFxMovieView::ScaleModeType a_scaleMode)
	{
		_SetViewScaleMode(a_view, a_scaleMode);

		IconSetter::Install(a_view, "_global.CraftingIconSetter.prototype");
		IconSetter::Install(a_view, "_global.InventoryIconSetter.prototype");
		IconSetter::Install(a_view, "_global.MagicIconSetter.prototype");
		IconSetter::Install(a_view, "_global.FavoritesIconSetter.prototype");

		InventoryListEntry::Install(a_view, "_global.InventoryListEntry.prototype");
		InventoryListEntry::Install(a_view, "_global.CraftingListEntry.prototype");
		FavoritesListEntry::Install(a_view, "_global.FavoritesListEntry.prototype");

		GroupDataExtender::Install(a_view, "_global.GroupDataExtender.prototype");
	}
}
