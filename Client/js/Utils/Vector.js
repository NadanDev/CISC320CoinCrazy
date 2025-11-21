class Vec extends Array {
  /**
   * Constructor for a vector.
   * 
   * @class
   * @global
   * @extends Array
   * @param {...number} args Multiple paramaters of numbers to form an vector
   *  or an array of numbers.
   * @example new Vec(1, 2, 3);
   * @example new Vec([1, 2, 3]);
   */
  constructor(...args) {
    super();
 
    if (args[0] instanceof Vec) args = args[0];
    for (let e of args) {
      this.push(e);
    }
  }
 
  /** 
   * Checks if object is a vector using "instanceof" keyword
   * 
   * @static
   * @param {Object} instance object to check
   * @returns {Boolean} true if its a vector(arrays count as vectors)
   * @example [1, 2, 3] instanceof Vec // returns true
   * @example ["1", '2', "0x3"] instanceof Vec // returns true
   * @example ["a", "b", "c"] instanceof Vec // returns false
   */
  static [Symbol.hasInstance](instance) {
    return Array.isArray(instance) && !instance.some(isNaN);
  }
  static isVector(instance) {
    return instance instanceof Vec;
  }
 
  /** 
   * Generate a unit vector from a direction
   * 
   * @static
   * @param {Number} dir direction of the unit vector
   * @returns {Vec}
   */
  static unit(dir) {
    return new Vec(Math.cos(dir), Math.sin(dir));
  }
 
  /**
   * Adds two or more vs/arrays and returns a new vector.
   *
   * @static
   * @param {...Vec | number[]} vs Multiple vectors or array of vectors to be added.
   * @returns {Vec} Sum of vs.
   * @example Vec.add([1, 2, 3], [4, 2]);
   */
  static add(...vs) {
    let final = new Vec();
    let maxLen = Math.max.apply(Math, vs.map(e => e.length));
    for (let i = 0; i < maxLen; i++) {
      final[i] = vs.reduce((s, e) => s + (e[i] || 0), 0);
    }
    return final;
  }
 
  /**
   * Subtracts two vs/arrays and returns a new vector.
   *
   * @static
   * @param {Vec | number[]} v1 Vector to be subtracted from.
   * @param {Vec | number[]} v2 Vector to be subtracted with.
   * @returns {Vec}
   * @example Vec.subtract([1, 2, 3], [4, 2]);
   */
  static subtract(v1, v2) {
    let final = new Vec();
    let maxLen = Math.max(v1.length, v2.length);
    for (let i = 0; i < maxLen; i++) {
      final[i] = (v1[i] || 0) - (v2[i] || 0);
    }
    return final;
  }
 
  /**
   * Scales a vector by a constant and returns a new vector
   *
   * @static
   * @param {Vec|number[]} v1 Vector to be subtracted from.
   * @param {number} s Amount to scale the vector by.
   * @returns {Vec}
   * @example Vec.scale([1, 2, 3], -1);
   */
  static scale(v, s) {
    return new Vec(v.map(e => e * s));
  }
 
  /**
   * Normalises a vector into a unit vector and returns a new vector.
   *
   * @static
   * @param {Vec|number[]} v1 Vector to be scaled.
   * @returns {Vec}
   * @example Vec.norm([1, 2, 3]);
   */
  static norm(v) {
    return this.scale(v, 1 / v.magn());
  }
 
  /**
   * Preforms dot product between two vectors.
   *
   * @static
   * @param {Vec|number[]} v1 First vector.
   * @param {Vec|number[]} v2 Second vector.
   * @returns {number}
   * @example Vec.dot([1, 2, 3], [4, 2]);
   */
  static dot(v1, v2) {
    if (v2.length > v1.length) {
      let temp = v1;
      v1 = v2;
      v2 = temp;
    }
    return v1.reduce((s, e, i) => s + e * (v2[i] || 0), 0);
  }
 
  /**
   * Preforms cross product between two vectors(3d).
   *
   * @static
   * @param {Vec|number[]} v1 First vector.
   * @param {Vec|number[]} v2 Second vector.
   * @returns {Vec}
   * @example Vec.cross([1, 2, 3], [4, 2]);
   */
  static cross(v1, v2) {
    return new Vec(
      v1[1] * v2[2] - v1[2] * v2[1],
      v1[2] * v2[0] - v1[0] * v2[2],
      v1[0] * v2[1] - v1[1] * v2[0],
    );
  }
 
  /**
   * Adds two or more vs/arrays to the current vector.
   *
   * @param {...Vec|number[]} vs Multiple vectors or array of vectors to be added.
   * @returns {Vec} this
   * @example this.add([1, 2, 3]);
   */
  add(...vs) {
    let maxLen = Math.max.apply(Math, vs.map(e => e.length));
    for (let i = 0; i < maxLen; i++) {
      this[i] += vs.reduce((s, e) => s + (e[i] || 0), 0);
    }
    return this;
  }
 
  /**
   * Subtract a vector from current vector.
   *
   * @param {Vec|number[]} v A vector to subtract with.
   * @returns {Vec} this
   * @example this.subtract([1, 2, 3]);
   */
  subtract(v) {
    let maxLen = Math.max(this.length, v.length);
    for (let i = 0; i < maxLen; i++) {
      this[i] = (this[i] || 0) - (v[i] || 0);
    }
    return this;
  }
 
  /**
   * Scale current vector by a constant.
   *
   * @param {number} s Amount to scale the vector by.
   * @returns {Vec} this
   * @example this.scale(-1);
   */
  scale(s) {
    for (let i = 0; i < this.length; i++) {
      this[i] *= s;
    }
    return this;
  }
 
  /**
   * Gets the magnitude of the vector.
   *
   * @returns {number} magnitude
   * @example this.magn();
   */
  magn() {
    return Math.hypot.apply(Math, this);
  }
 
  /**
   * Normalises current vector.
   *
   * @returns {Vec} this
   * @example this.norm();
   */
  norm() {
    this.scale(1 / this.magn());
    return this;
  }
 
  /** 
   * Copies the elements of another vector into this vector.
   * 
   * @param 
   * @returns {Vec} this
   * @example this.copy(v);
   */
  copy(v) {
    this.length = 0;
    for (let e of v) {
      this.push(e);
    }
    return this;
  }
 
  /**
   * Gets the x value of the current vector.
   * 
   * @returns {number} number at element 0.
   * @example this.x
   */
  get x() {
    return this[0];
  }
 
  /**
   * Gets the y value of the current vector.
   * 
   * @returns {number} number at element 1.
   * @example this.y
   */
  get y() {
    return this[1];
  }
 
  /**
   * Gets the z value of the current vector.
   * 
   * @returns {number} number at element 2.
   * @example this.z
   */
  get z() {
    return this[2];
  }
 
  /**
   * Sets the x value of the current vector.
   * 
   * @param {number} num value to set at index 0.
   * @example this.x
   */
  set x(num) {
    this[0] = num;
  }
 
  /**
   * Sets the y value of the current vector.
   * 
   * @param {number} num value to set at index 1.
   * @example this.y
   */
  set y(num) {
    this[1] = num;
  }
 
  /**
   * Sets the z value of the current vector.
   * 
   * @param {number} num value to set at index 2.
   * @example this.z
   */
  set z(num) {
    this[2] = num;
  }
 
  /** 
   * Checks for equality between the two objects.
   *
   * @param {Object} obj The object which to compare with.
   * @returns {boolean} True if the two objects are logically equal.
   */
  equals(obj) {
    if (!(obj instanceof Vec)) {
      return false;
    }
    return this.length > obj.length ? (
      this.every((e, i) => e == obj[i] || 0)
    ) : (
      obj.every((e, i) => e == this[i] || 0)
    );
  }
 
  /**
   * Type coersion handling
   * 
   * @param {Object} obj type the type to coerce to
   * @throws {Error} when attempting to coerce to number(vector used in math incorrectly)
   * @example new Vec(1, 2, 3) + new Vec(4, 5, 6);
   */
  [Symbol.toPrimitive](type) {
    if (type == "string") {
      return this.toString();
    }
    if (type == "default") {
      return true;
    }
    throw new Error("vector used in math improperly");
  }
}
 
 
//exports
export { Vec as default, Vec };
(typeof self == "undefined" ? global : self).Vec = Vec;
if (typeof module != "undefined")
  module.exports = { Vec };