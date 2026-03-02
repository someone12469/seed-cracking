package blockcoords.mixin;

import net.minecraft.client.color.block.BlockColors;
import net.minecraft.client.multiplayer.ClientLevel;
import net.minecraft.core.BlockPos;
import net.minecraft.core.Direction;
import net.minecraft.server.MinecraftServer;
import net.minecraft.world.level.BlockAndTintGetter;
import net.minecraft.world.level.block.state.BlockState;
import net.minecraft.world.level.Level;
import org.jetbrains.annotations.Nullable;
import org.spongepowered.asm.mixin.injection.At;
import org.spongepowered.asm.mixin.injection.callback.CallbackInfoReturnable;
import org.spongepowered.asm.mixin.injection.Inject;
import org.spongepowered.asm.mixin.Mixin;
import org.spongepowered.asm.mixin.Overwrite;

@Mixin(BlockColors.class)
class ExampleMixin {
	@Inject(method = "getColor(Lnet/minecraft/world/level/block/state/BlockState;Lnet/minecraft/world/level/BlockAndTintGetter;Lnet/minecraft/core/BlockPos;I)I", at = @At("HEAD"), cancellable = true)
	private void inj2(BlockState blockState, @Nullable BlockAndTintGetter blockAndTintGetter, @Nullable BlockPos blockPos, int i, CallbackInfoReturnable<Integer> cir) {
		int ret = 0;
		if(blockPos != null)
			ret = (blockPos.getX() % 256 + 256) % 256 * 65536 + (blockPos.getY() % 256 + 256) % 256 * 256 + ((blockPos.getZ() % 256) + 256) % 256;
		cir.setReturnValue(ret);
		cir.cancel();
	}
}

@Mixin(ClientLevel.class)
class LevelMixin {
	@Overwrite
	public float getShade(Direction dir, boolean shade) {
		return 1.0f;
	}
}
// would make sense to also make all quads tinted here but that seemed too difficult to do without crashing the game and was not necessary
