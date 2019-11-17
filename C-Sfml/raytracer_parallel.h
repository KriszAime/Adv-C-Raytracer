// raytracer multi thread rays
// A program több processzor magot is használ, a renderelés gyorsítására.
// Illetve, a program kerüli, a felesleges ugrásokat, tényleges rekurziót.
// Everything done by the CPU.
// prog: Elric Christopher

void ray_parallel(Ray_Threading *rp)
{
	int tmp = rp->Th_Div + ((rp->rt->resolutionX * rp->rt->resolutionY) / rp->rt->_Threading._nprocs);
	if (rp->rt->_Threading._nprocs <= 1) tmp = (rp->rt->resolutionX * rp->rt->resolutionY);
	do
	{
		for (int s = rp->Th_Div; s < tmp; s++) {
			if (*rp->_continue == false) break;
			//--ray_makeForPixel
			Ray ray; _ray_makeForPixel(ray, rp->rt->scene.camera, rp->hwrnd[s].x, rp->hwrnd[s].y);
			//--
			//------TRACING....
			//--ray trace once
			TracingResult closestHit; _ray_traceOnce(closestHit, ray, rp->rt->scene);
			//-
			Color resultColor;
			if (closestHit.surface == NULL) {
				resultColor = rp->rt->scene.backgroundColor;
			}
			else
			{
				resultColor = closestHit.surface->material.color;
				Vector3 collisionPoint, tmult; VEC3_MULT(tmult, ray.direction, closestHit.distance);
				VEC3_ADD(collisionPoint, tmult, ray.origin);
				//-
				if (closestHit.surface->material.reflectivity > 0.0 && MAX_RECURSION_DEPTH > 0) {
					Ray reflectedRay; _ray_reflect(reflectedRay, ray, closestHit.surface, collisionPoint);
					//if (closestHit.surface->material.reflectionNoise > 0) {
					//	reflectedRay = ray_addNoise(&reflectedRay, closestHit.surface->material.reflectionNoise); //someting is isn't right with this orig funct. it geves back the original no matter what?
					//	//_ray_addNoise(reflectedRay, closestHit.surface->material.reflectionNoise);
					//}
					//----Ray Trace Depth -1
					Color reflectionColor;
					{
						TracingResult closestHit; _ray_traceOnce(closestHit, reflectedRay, rp->rt->scene);
						if (closestHit.surface == NULL) { reflectionColor = rp->rt->scene.backgroundColor; }
						else
						{
							reflectionColor = closestHit.surface->material.color;
							Vector3 collisionPoint, tmult; VEC3_MULT(tmult, reflectedRay.direction, closestHit.distance);
							VEC3_ADD(collisionPoint, tmult, reflectedRay.origin);
							//-
							if (closestHit.surface->material.reflectivity > 0.0 && MAX_RECURSION_DEPTH > 1) {
								Ray reflectedRay1; _ray_reflect(reflectedRay1, reflectedRay, closestHit.surface, collisionPoint);
								//if (closestHit.surface->material.reflectionNoise > 0) {
								//	reflectedRay1 = ray_addNoise(&reflectedRay1, closestHit.surface->material.reflectionNoise); //someting is isn't right with this orig funct. it geves back the original no matter what?
								//	//_ray_addNoise(reflectedRay, closestHit.surface->material.reflectionNoise);
								//}
								//--Ray trace Depth -2
								Color reflectionColor2;
								{
									TracingResult closestHit; _ray_traceOnce(closestHit, reflectedRay1, rp->rt->scene);
									if (closestHit.surface == NULL) { reflectionColor2 = rp->rt->scene.backgroundColor; }
									else
									{
										reflectionColor2 = closestHit.surface->material.color;
										Vector3 collisionPoint, tmult; VEC3_MULT(tmult, reflectedRay1.direction, closestHit.distance);
										VEC3_ADD(collisionPoint, tmult, reflectedRay1.origin);
										//{ next layer...
										//}
										//--shadingResult
										ShadingResult shadingResult;
										_ray_shadeAtPoint(shadingResult, reflectedRay1, rp->rt->scene, closestHit.surface, collisionPoint);
										//--
										_getHighlightedColor(reflectionColor2, shadingResult, rp->rt->scene.ambientCoefficient);
										_color_mult(reflectionColor2, (MAX_VISIBLE_DISTANCE - closestHit.distance) / MAX_VISIBLE_DISTANCE);
									}
								}
								//---
								_color_blend(reflectionColor, reflectionColor2, closestHit.surface->material.reflectivity, reflectionColor);
							}
							//-
							//--shadingResult
							ShadingResult shadingResult;
							_ray_shadeAtPoint(shadingResult, reflectedRay, rp->rt->scene, closestHit.surface, collisionPoint);
							//--
							_getHighlightedColor(reflectionColor, shadingResult, rp->rt->scene.ambientCoefficient);
							_color_mult(reflectionColor, (MAX_VISIBLE_DISTANCE - closestHit.distance) / MAX_VISIBLE_DISTANCE);
						}
					}
					//---
					_color_blend(resultColor, reflectionColor, closestHit.surface->material.reflectivity, resultColor);
				}
				//-
				//--shadingResult
				ShadingResult shadingResult;
				_ray_shadeAtPoint(shadingResult, ray, rp->rt->scene, closestHit.surface, collisionPoint);
				//--
				_getHighlightedColor(resultColor, shadingResult, rp->rt->scene.ambientCoefficient);
				_color_mult(resultColor, (MAX_VISIBLE_DISTANCE - closestHit.distance) / MAX_VISIBLE_DISTANCE);
			}
			//--
			sfImage_setPixel((sfImage*)rp->data, rp->hwrnd[s].x, rp->hwrnd[s].y, (sfColor) {
				.r = (sfUint8)(resultColor.r * 255.0),
					.g = (sfUint8)(resultColor.g * 255.0),
					.b = (sfUint8)(resultColor.b * 255.0),
					.a = 255
			});

		}
	} while (*rp->_continue == true);
	return;
}