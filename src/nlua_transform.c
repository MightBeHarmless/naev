/*
 * See Licensing and Copyright notice in naev.h
 */

/**
 * @file nlua_transform.c
 *
 * @brief Handles transforms.
 */

/** @cond */
#include <lauxlib.h>

#include "naev.h"
/** @endcond */

#include "nlua_transform.h"

#include "log.h"
#include "ndata.h"
#include "nluadef.h"


/* Transform metatable methods. */
static int transformL_eq( lua_State *L );
static int transformL_new( lua_State *L );
static int transformL_mul( lua_State *L );
static int transformL_get( lua_State *L );
static int transformL_scale( lua_State *L );
static int transformL_translate( lua_State *L );
static int transformL_rotate2d( lua_State *L );
static int transformL_applyPoint( lua_State *L );
static int transformL_applyDim( lua_State *L );
static const luaL_Reg transformL_methods[] = {
   { "__eq", transformL_eq },
   { "__mul", transformL_mul },
   { "get", transformL_get },
   { "new", transformL_new },
   { "scale", transformL_scale },
   { "translate", transformL_translate },
   { "rotate2d", transformL_rotate2d },
   { "applyPoint", transformL_applyPoint },
   { "applyDim", transformL_applyDim },
   {0,0}
}; /**< Transform metatable methods. */


/**
 * @brief Loads the transform library.
 *
 *    @param env Environment to load transform library into.
 *    @return 0 on success.
 */
int nlua_loadTransform( nlua_env env )
{
   nlua_register(env, TRANSFORM_METATABLE, transformL_methods, 1);
   return 0;
}


/**
 * @brief Lua bindings to interact with transforms.
 *
 * @luamod transform
 */
/**
 * @brief Gets transform at index.
 *
 *    @param L Lua state to get transform from.
 *    @param ind Index position to find the transform.
 *    @return Transform found at the index in the state.
 */
gl_Matrix4* lua_totransform( lua_State *L, int ind )
{
   return (gl_Matrix4*) lua_touserdata(L,ind);
}
/**
 * @brief Gets transform at index or raises error if there is no transform at index.
 *
 *    @param L Lua state to get transform from.
 *    @param ind Index position to find transform.
 *    @return Transform found at the index in the state.
 */
gl_Matrix4* luaL_checktransform( lua_State *L, int ind )
{
   if (lua_istransform(L,ind))
      return lua_totransform(L,ind);
   luaL_typerror(L, ind, TRANSFORM_METATABLE);
   return NULL;
}
/**
 * @brief Pushes a transform on the stack.
 *
 *    @param L Lua state to push transform into.
 *    @param transform Transform to push.
 *    @return Newly pushed transform.
 */
gl_Matrix4* lua_pushtransform( lua_State *L, gl_Matrix4 transform )
{
   gl_Matrix4 *t;
   t = (gl_Matrix4*) lua_newuserdata(L, sizeof(gl_Matrix4));
   *t = transform;
   luaL_getmetatable(L, TRANSFORM_METATABLE);
   lua_setmetatable(L, -2);
   return t;
}
/**
 * @brief Checks to see if ind is a transform.
 *
 *    @param L Lua state to check.
 *    @param ind Index position to check.
 *    @return 1 if ind is a transform.
 */
int lua_istransform( lua_State *L, int ind )
{
   int ret;

   if (lua_getmetatable(L,ind)==0)
      return 0;
   lua_getfield(L, LUA_REGISTRYINDEX, TRANSFORM_METATABLE);

   ret = 0;
   if (lua_rawequal(L, -1, -2))  /* does it have the correct mt? */
      ret = 1;

   lua_pop(L, 2);  /* remove both metatables */
   return ret;
}


/**
 * @brief Compares two transforms to see if they are the same.
 *
 *    @luatparam Transform t1 Transform 1 to compare.
 *    @luatparam Transform t2 Transform 2 to compare.
 *    @luatreturn boolean true if both transforms are the same.
 * @luafunc __eq( t1, t2 )
 */
static int transformL_eq( lua_State *L )
{
   gl_Matrix4 *t1, *t2;
   t1 = luaL_checktransform(L,1);
   t2 = luaL_checktransform(L,2);
   lua_pushboolean( L, (memcmp( t1, t2, sizeof(gl_Matrix4) )==0) );
   return 1;
}


/**
 * @brief Creates a new identity transform.Gets a transform.
 *
 *    @luatreturn Transform A new transform corresponding to an identity matrix.
 * @luafunc new( )
 */
static int transformL_new( lua_State *L )
{
   gl_Matrix4 *M;
   if (lua_istransform(L,1)) {
      M = lua_totransform(L,1);
      lua_pushtransform( L, *M );
   }
   else
      lua_pushtransform( L, gl_Matrix4_Identity() );
   return 1;
}


/**
 * @brief Multiplies two transforms (A*B).
 *
 *    @luatparam Transform A First element to multiply.
 *    @luatparam Transform B Second element to multiply.
 *    @luatreturn Transform Result of multiplication.
 * @luafunc __mul( A, B )
 */
static int transformL_mul( lua_State *L )
{
   gl_Matrix4 *A = luaL_checktransform(L, 1);
   gl_Matrix4 *B = luaL_checktransform(L, 2);
   gl_Matrix4 C = gl_Matrix4_Mult( *A, *B );
   lua_pushtransform(L, C);
   return 1;
}


/**
 * @brief Gets all the values of the transform.
 *
 *    @luatparam Transform T Transform te get parameters of.
 *    @luatreturn table 2D table containing all the values of the transform.
 * @luafunc get( T )
 */
static int transformL_get( lua_State *L )
{
   gl_Matrix4 *M = luaL_checktransform(L, 1);
   int i,j;
   lua_newtable(L);              /* t */
   for (i=0; i<4; i++) {
      lua_pushinteger(L,i+1);    /* t, n */
      lua_newtable(L);           /* t, n, t */
      for (j=0; j<4; j++) {
         lua_pushinteger(L,j+1); /* t, n, t, n */
         lua_pushnumber(L,M->m[i][j]); /* t, n, t, n, n */
         lua_rawset(L,-3);       /* t, n, t */
      }
      lua_rawset(L,-3);          /* t */
   }
   return 1;
}


/**
 * @brief Applies scaling to a transform.
 *
 *    @luatparam Transform T Transform to apply scaling to.
 *    @luatparam number x X-axis scaling.
 *    @luatparam number y Y-axis scaling.
 *    @luatparam number z Z-axis scaling.
 *    @luatreturn Transform A new transformation.
 * @luafunc scale( T, x, y, z )
 */
static int transformL_scale( lua_State *L )
{
   gl_Matrix4 *M = luaL_checktransform(L, 1);
   double x = luaL_checknumber(L,2);
   double y = luaL_checknumber(L,3);
   double z = luaL_checknumber(L,4);
   lua_pushtransform(L, gl_Matrix4_Scale( *M, x, y, z ) );
   return 1;
}


/**
 * @brief Applies translation to a transform.
 *
 *    @luatparam Transform T Transform to apply translation to.
 *    @luatparam number x X-axis translation.
 *    @luatparam number y Y-axis translation.
 *    @luatparam number z Z-axis translation.
 *    @luatreturn Transform A new transformation.
 * @luafunc translate( T, x, y, z )
 */
static int transformL_translate( lua_State *L )
{
   gl_Matrix4 *M = luaL_checktransform(L, 1);
   double x = luaL_checknumber(L,2);
   double y = luaL_checknumber(L,3);
   double z = luaL_checknumber(L,4);
   lua_pushtransform(L, gl_Matrix4_Translate( *M, x, y, z ) );
   return 1;
}


/**
 * @brief Applies a 2D rotation (along Z-axis) to a transform.
 *
 *    @luatparam Transform T Transform to apply rotation to.
 *    @luatparam number angle Angle to rotate (radians).
 * @luafunc rotate2d( T, angle )
 */
static int transformL_rotate2d( lua_State *L )
{
   gl_Matrix4 *M = luaL_checktransform(L, 1);
   double a = luaL_checknumber(L,2);
   lua_pushtransform(L, gl_Matrix4_Rotate2d( *M, a ) );
   return 1;
}


/**
 * @brief Applies a trasnformation to a point.
 *
 *    @luatparam Transform T Transform to apply.
 *    @luatparam number x Point X-coordinate.
 *    @luatparam number y Point Y-coordinate.
 *    @luatparam number z Point Z-coordinate.
 *    @luatreturn number New X coordinate.
 *    @luatreturn number New Y coordinate.
 *    @luatreturn number New Z coordinate.
 * @luafunc applyPoint( T, x, y, z )
 */
static int transformL_applyPoint( lua_State *L )
{
   gl_Matrix4 *M;
   double gp[3], p[3];
   int i;
   M = luaL_checktransform(L, 1);
   gp[0] = luaL_checknumber(L,2);
   gp[1] = luaL_checknumber(L,3);
   gp[2] = luaL_checknumber(L,4);

   for (i=0; i<3; i++)
      p[i] = M->m[0][i]*gp[0] + M->m[1][i]*gp[1] + M->m[2][i]*gp[2] + M->m[3][i];

   lua_pushnumber(L, p[0]);
   lua_pushnumber(L, p[1]);
   lua_pushnumber(L, p[2]);
   return 3;
}


/**
 * @brief Applies a trasnformation to a dimension.
 *
 * @note This is similar to Transform.applyPoint, except the translation is not applied.
 *
 *    @luatparam Transform T Transform to apply.
 *    @luatparam number x Dimension X-coordinate.
 *    @luatparam number y Dimension Y-coordinate.
 *    @luatparam number z Dimension Z-coordinate.
 *    @luatreturn number New X coordinate.
 *    @luatreturn number New Y coordinate.
 *    @luatreturn number New Z coordinate.
 * @luafunc applyDim( T, x, y, z )
 */
static int transformL_applyDim( lua_State *L )
{
   gl_Matrix4 *M;
   double gp[3], p[3];
   int i;
   M = luaL_checktransform(L, 1);
   gp[0] = luaL_checknumber(L,2);
   gp[1] = luaL_checknumber(L,3);
   gp[2] = luaL_checknumber(L,4);

   for (i=0; i<3; i++)
      p[i] = M->m[0][i]*gp[0] + M->m[1][i]*gp[1] + M->m[2][i]*gp[2];

   lua_pushnumber(L, p[0]);
   lua_pushnumber(L, p[1]);
   lua_pushnumber(L, p[2]);
   return 3;
}
