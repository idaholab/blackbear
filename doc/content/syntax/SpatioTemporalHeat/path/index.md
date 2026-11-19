# SpatioTemporalHeat/path

The `[path]` sub-block of [SpatioTemporalHeat](syntax/SpatioTemporalHeat/index.md) defines the
spatio-temporal path that the heat source follows. It takes a
[SpatioTemporalPath](SpatioTemporalPath.md) with the same parameters as in the
[SpatioTemporalPaths](syntax/SpatioTemporalPaths/index.md) block. The path is named
`SpatioTemporalHeat_path`.

!listing test/tests/spatiotemporal_path/esm_hsource.i block=SpatioTemporalHeat/path

!syntax list /SpatioTemporalHeat/path objects=True actions=False subsystems=False

!syntax list /SpatioTemporalHeat/path objects=False actions=True subsystems=False
